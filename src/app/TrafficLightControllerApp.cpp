#include "impl/app/TrafficLightControllerApp.hpp"

#include <iostream>
#include <utility>

TrafficLightControllerApp::TrafficLightControllerApp
(
    const Clock &clockRef,
    const VehicleSensors &sensorsRef,
    IClock::Time maxWaitTime
)
    : clock_(clockRef),
      sensors_(sensorsRef),
      signals_(),
      appState_(false),
      carsAwaiting_(false),
      maxWaitTime_(maxWaitTime),
      lightStates_(),
      vehicleStates_(),
      laneToOpposingLaneMap_(),
      patternToLaneMap_()
{
    std::cout << "Constructed TrafficLightControllerApp." << std::endl;
}

void TrafficLightControllerApp::initApp()
{
    makePatternToLaneMap();
    makeOpposingLanesMap();
    populateLightStates();
    populateVehicleStates();

    /// Start the controller in the NorthSouthTurning Pattern
    /// as specified by the requirements.
    enableNorthSouthTurning(lightStates_[TrafficLightPattern::NorthSouthTurning]);

    appState_ = true;

    std::cout << "Initialized TrafficLightControllerApp." << std::endl;
}

void TrafficLightControllerApp::run()
{
    /// Only process data if app is in good state.
    /// Ideally, this appState_ memeber would be eliminated
    /// with the use of an ApplicationManagerApp.
    if (appState_)
    {
        checkCycleState();

        processVehicleSensors();
    }
}

void TrafficLightControllerApp::checkCycleState()
{
    for (int currentLight = 0; currentLight < TrafficLightPattern::NUM_PATTERNS; currentLight++)
    {
        TrafficLightState &currentlightState = lightStates_[currentLight];

        // Case 1: Light on, lanes clear, do nothing
        if (currentlightState.isOn && currentlightState.areOpposingLanesClear)
        {
            break;
        }

        // Case 2: Light on, check light active time vs. min/max light active times
        if (currentlightState.isOn)
        {
            currentlightState.activeTime = clock_.elapsed(currentlightState.startTime);

            if (currentlightState.activeTime >= currentlightState.minActiveTime && carsAwaiting_)
            {
                nextPattern(currentLight);
            }

            if (currentlightState.activeTime >= currentlightState.maxActiveTime)
            {
                nextPattern(currentLight);
            }
        }
        
        // Case 3: Light off, but lanes clear, advance to next pattern
        if (!currentlightState.isOn && currentlightState.areOpposingLanesClear)
        {
            for (int nextLight = 0; nextLight < TrafficLightPattern::NUM_PATTERNS; nextLight++)
            {
                TrafficLightState &nextlightState = lightStates_[nextLight];

                if (nextlightState.isOn)
                {
                    nextlightState.activeTime = clock_.elapsed(nextlightState.startTime);

                    if (nextlightState.activeTime >= nextlightState.minActiveTime)
                    {
                        nextPattern(nextLight);
                    }
                }
            }
        }
    }
}

void TrafficLightControllerApp::processVehicleSensors()
{
    for (int lane = 0; lane < Lane::COUNT; lane++)
    {
        SensorState sensorState = sensors_[lane];
        SignalState signalState = signals_[lane];
        VehicleState &vehicleState = vehicleStates_[lane];

        bool vehicleWillWait = (sensorState == SensorState::SET && signalState == SignalState::RED);
        bool vehicleWillProceed = (sensorState == SensorState::SET && signalState == SignalState::GREEN);
        
        if (vehicleWillWait)
        {
            processVehicleAtRed(vehicleState);
        }
        else if (vehicleWillProceed)
        {
            processVehicleAtGreen(vehicleState);
        }
        else
        {
            vehicleState.isWaiting = false;
            vehicleState.arrivalTime = 0;
            vehicleState.waitTime = 0;
        }
    }

    checkIfCarsAreWaiting();
}

void TrafficLightControllerApp::nextPattern(int patternIndex)
{
    if (patternIndex == 3)
    {
        updateCycle(lightStates_[0]);
    }
    else
    {
        updateCycle(lightStates_[patternIndex + 1]);
    }
}

void TrafficLightControllerApp::processVehicleAtRed(VehicleState &vehicleState)
{
    std::cout << "Car in lane (" << laneToString(vehicleState.lane) << ") waiting at RED light." <<std::endl;

    checkOpposingLanes(vehicleState.lane);
    checkWaitTime(vehicleState);
}

void TrafficLightControllerApp::processVehicleAtGreen(VehicleState &vehicleState)
{
    std::cout << "Car in lane (" << laneToString(vehicleState.lane) << ") proceeding with GRN light."
        " Impatient driver waited (" << vehicleState.waitTime << "s)." << std::endl;

    checkOpposingLanes(vehicleState.lane);

    vehicleState.isWaiting = false;
    vehicleState.arrivalTime = 0;
    vehicleState.waitTime = 0;
}

void TrafficLightControllerApp::updateCycle(TrafficLightState &lightState)
{
    switch (lightState.pattern)
    {
        case TrafficLightPattern::NorthSouthTurning:
            enableNorthSouthTurning(lightState);
            break;
        
        case TrafficLightPattern::NorthSouthThrough:
            enableNorthSouthThrough(lightState);
            break;

        case TrafficLightPattern::EastWestTurning:
            enableEastWestTurning(lightState);
            break;

        case TrafficLightPattern::EastWestThrough:
            enableEastWestThrough(lightState);
            break;

        default:
            std::cout << "Invalid pattern! Check back when you are in a flying car!" << std::endl;
            break;
    }
}

void TrafficLightControllerApp::enableNorthSouthTurning(TrafficLightState &lightState)
{
    disablePattern(lightStates_[TrafficLightPattern::EastWestThrough]);
    setLaneSignal(Lane::E_E, SignalState::RED);
    setLaneSignal(Lane::W_W, SignalState::RED);

    enablePattern(lightState);
    setLaneSignal(Lane::N_W, SignalState::GREEN);
    setLaneSignal(Lane::S_E, SignalState::GREEN);
}

void TrafficLightControllerApp::enableNorthSouthThrough(TrafficLightState &lightState)
{
    disablePattern(lightStates_[TrafficLightPattern::NorthSouthTurning]);
    setLaneSignal(Lane::N_W, SignalState::RED);
    setLaneSignal(Lane::S_E, SignalState::RED);

    enablePattern(lightState);
    setLaneSignal(Lane::N_N, SignalState::GREEN);
    setLaneSignal(Lane::S_S, SignalState::GREEN);
}

void TrafficLightControllerApp::enableEastWestTurning(TrafficLightState &lightState)
{
    disablePattern(lightStates_[TrafficLightPattern::NorthSouthThrough]);
    setLaneSignal(Lane::N_N, SignalState::RED);
    setLaneSignal(Lane::S_S, SignalState::RED);

    enablePattern(lightState);
    setLaneSignal(Lane::E_N, SignalState::GREEN);
    setLaneSignal(Lane::W_S, SignalState::GREEN);
}

void TrafficLightControllerApp::enableEastWestThrough(TrafficLightState &lightState)
{
    disablePattern(lightStates_[TrafficLightPattern::EastWestTurning]);
    setLaneSignal(Lane::E_N, SignalState::RED);
    setLaneSignal(Lane::W_S, SignalState::RED);

    enablePattern(lightState);
    setLaneSignal(Lane::E_E, SignalState::GREEN);
    setLaneSignal(Lane::W_W, SignalState::GREEN);
}

void TrafficLightControllerApp::enablePattern(TrafficLightState &lightState)
{
    lightState.isOn = true;
    lightState.startTime = clock_.now();
    lightState.activeTime = 0;

    std::cout << lightPatternToString(lightState.pattern) << " Enabled." << std::endl;
}

void TrafficLightControllerApp::disablePattern(TrafficLightState &lightState)
{
    lightState.isOn = false;
    lightState.startTime = 0;
    lightState.activeTime = 0;

    std::cout << lightPatternToString(lightState.pattern) << " Disabled." << std::endl;
}

void TrafficLightControllerApp::checkOpposingLanes(Lane lane)
{
    bool areOpposingLanesClear = true;

    auto opposingLanes = laneToOpposingLaneMap_[lane];
    
    for(auto i : opposingLanes)
    {
        if (i != Lane::PLACEHOLDER && sensors_[i] == SensorState::SET)
        {
            areOpposingLanesClear = false;
            break;
        }
    }

    notifyOpposingLanesClear(lane, areOpposingLanesClear);
}

void TrafficLightControllerApp::notifyOpposingLanesClear(Lane lane, bool isClear)
{
    if (isClear)
    {
        std::cout << "Opposing lanes are clear for lane (" << laneToString(lane) << ")." << std::endl;
    }

    for (auto keyPattern : patternToLaneMap_)
    {
        auto lanesInPattern = keyPattern.second;
        for (auto laneInPattern : lanesInPattern)
        { 
            if (laneInPattern == lane)
            {
                lightStates_[keyPattern.first].areOpposingLanesClear = isClear;
            }
        }
    }
}

void TrafficLightControllerApp::checkWaitTime(VehicleState &vehicleState)
{
    if (!vehicleState.isWaiting)
    {
        vehicleState.isWaiting = true;
        vehicleState.arrivalTime = clock_.now();
    }
    else
    {
        vehicleState.waitTime = clock_.elapsed(vehicleState.arrivalTime);
    }
}

void TrafficLightControllerApp::checkIfCarsAreWaiting()
{
    int isWaitingCounter = 0;

    for (auto vs : vehicleStates_)
    {
        if (vs.isWaiting)
        {
            isWaitingCounter++;
            break;
        }
    }

    if (isWaitingCounter == 0)
    {
        carsAwaiting_ = false;
    }
    else
    {
        carsAwaiting_ = true;
    }
}

void TrafficLightControllerApp::setLaneSignal(Lane lane, SignalState state)
{
    signals_[lane] = state;
}
