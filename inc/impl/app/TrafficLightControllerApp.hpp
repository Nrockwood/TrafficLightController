#ifndef INCLUDE_TRAFFICLIGHTCONTROLLERAPP_H_
#define INCLUDE_TRAFFICLIGHTCONTROLLERAPP_H_

#include "interfaces/app/IApp.hpp"
#include "interfaces/clock/IClock.hpp"

#include "impl/simulator/simulator.hpp"

#include <cstdint>
#include <map>

////////////////////////////////////////////////////////////
///  @brief The possible patterns of the traffic light
///  
////////////////////////////////////////////////////////////
enum TrafficLightPattern
{
    NorthSouthTurning, ///< State 0 for N_W & S_E turning
    NorthSouthThrough, ///< State 1 for N_N & S_S through
    EastWestTurning,   ///< State 2 for E_N & W_S turning
    EastWestThrough,   ///< State 3 for E_E & W_W through
    NUM_PATTERNS
};

////////////////////////////////////////////////////////////
///  @brief Traffic Light on/off and time tracking
///  
////////////////////////////////////////////////////////////
struct TrafficLightState
{
    TrafficLightPattern pattern;
    bool isOn;
    bool areOpposingLanesClear;
    IClock::Time startTime;
    IClock::Time activeTime;
    IClock::Time minActiveTime;
    IClock::Time maxActiveTime;
};

////////////////////////////////////////////////////////////
///  @brief Vehicle arrival and waiting time
///  
////////////////////////////////////////////////////////////
struct VehicleState
{
    Lane lane;
    bool isWaiting;
    IClock::Time arrivalTime;
    IClock::Time waitTime;
};

////////////////////////////////////////////////////////////
///  @brief TrafficLightControllerApp controls a four-way
///  traffic light intersection.
///  
////////////////////////////////////////////////////////////
class TrafficLightControllerApp : public IApp
{
public:
    ////////////////////////////////////////////////////////////
    ///  @brief Construct a new Traffic Light Controller App object
    ///  
    ///  @param clockRef Reference to a Clock
    ///  @param sensorsRef Reference to a VehicleSensors
    ///  @param laneActiveTimesRef Reference to a LaneActiveTimes
    ///  @param maxWaitTime Max wait time for a vehicle at red light
    ////////////////////////////////////////////////////////////
    TrafficLightControllerApp(const Clock &clockRef,
                              const VehicleSensors &sensorsRef,
                              IClock::Time maxWaitTime);

    ////////////////////////////////////////////////////////////
    ///  @brief Initialize all necessary members for this app.
    ///  
    ////////////////////////////////////////////////////////////
    void initApp() override;

    ////////////////////////////////////////////////////////////
    ///  @brief Execute the logic of this app when called upon.
    ///  
    ////////////////////////////////////////////////////////////
    void run() override;

    ////////////////////////////////////////////////////////////
    ///  @brief Get the TrafficSignals object
    ///  
    ///  @return const TrafficSignals& The TrafficSignals
    ////////////////////////////////////////////////////////////
    inline const TrafficSignals& getSignals() const
    {
        return signals_;
    }

private:
    ////////////////////////////////////////////////////////////
    ///  @brief Checks the state of the controller.
    ///
    ///  Checks and advances the pattern:
    ///    - if the current pattern activeTime is over the minimum and maximum active times.
    ///    - if there are cars waiting at red lights
    ///    - if opposing lanes are clear 
    ///  
    ////////////////////////////////////////////////////////////
    void checkCycleState();

    ////////////////////////////////////////////////////////////
    ///  @brief Process SensorStates, SignalState, VehicleStates
    ///
    ///  If vehicle hits red light, it notifies the controller
    ///  that it is waiting or that opposing lanes are clear.
    ///  
    ////////////////////////////////////////////////////////////
    void processVehicleSensors();

    ////////////////////////////////////////////////////////////
    ///  @brief Advances to next light pattern iteration.
    ///  
    ///  @param patternIndex Pattern to advance to
    ////////////////////////////////////////////////////////////
    void nextPattern(int patternIndex);

    ////////////////////////////////////////////////////////////
    ///  @brief Process a vehicle queued at a red light.
    ///  
    ///  @param vehicleState Reference to a VehicleState
    ////////////////////////////////////////////////////////////
    void processVehicleAtRed(VehicleState &vehicleState);

    ////////////////////////////////////////////////////////////
    ///  @brief Process a vehicle with a green light.
    ///  
    ///  @param vehicleState Reference to a VehicleState
    ////////////////////////////////////////////////////////////
    void processVehicleAtGreen(VehicleState &vehicleState);

    ////////////////////////////////////////////////////////////
    ///  @brief Go to the next pattern in the cycle.
    ///  
    ///  @param lightState Reference to a TrafficLightState
    ////////////////////////////////////////////////////////////
    void updateCycle(TrafficLightState &lightState);

    ////////////////////////////////////////////////////////////
    ///  @brief Enables North & South Turning Pattern.
    ///
    ///  @param lightState Reference to a TrafficLightState  
    ////////////////////////////////////////////////////////////
    void enableNorthSouthTurning(TrafficLightState &lightState);

    ////////////////////////////////////////////////////////////
    ///  @brief Enables North & South Through Pattern.
    ///
    ///  @param lightState Reference to a TrafficLightState 
    ////////////////////////////////////////////////////////////
    void enableNorthSouthThrough(TrafficLightState &lightState);

    ////////////////////////////////////////////////////////////
    ///  @brief Enables East & West Turning Pattern.
    ///
    ///  @param lightState Reference to a TrafficLightState  
    ////////////////////////////////////////////////////////////
    void enableEastWestTurning(TrafficLightState &lightState);

    ////////////////////////////////////////////////////////////
    ///  @brief Enables East & West Through Pattern.
    ///
    ///  @param lightState Reference to a TrafficLightState  
    ////////////////////////////////////////////////////////////
    void enableEastWestThrough(TrafficLightState &lightState);

    ////////////////////////////////////////////////////////////
    ///  @brief Turns on the light and sets startTime to
    ///  current clock time.
    ///  
    ///  @param lightState Reference to a TrafficLightState  
    ////////////////////////////////////////////////////////////
    void enablePattern(TrafficLightState &lightState);

    ////////////////////////////////////////////////////////////
    ///  @brief Turns off the light.
    ///  
    ///  @param lightState Reference to a TrafficLightState  
    ////////////////////////////////////////////////////////////
    void disablePattern(TrafficLightState &lightState);

    ////////////////////////////////////////////////////////////
    ///  @brief Checks if the Lanes opposing lanes are clear.
    ///  
    ///  @param lane The lane to check
    ////////////////////////////////////////////////////////////
    void checkOpposingLanes(Lane lane);

    ////////////////////////////////////////////////////////////
    ///  @brief Find which pattern this lane maps to and
    ///  notify the system that lanes are clear.
    ///  
    ///  @param lane The Lane to search for
    ///  @param isClear If opposing lanes are clear or not
    ////////////////////////////////////////////////////////////
    void notifyOpposingLanesClear(Lane lane, bool isClear);

    ////////////////////////////////////////////////////////////
    ///  @brief Increments vehicles wait time and checks if
    ///  it maxWaitTime has been hit.
    ///  
    ///  @param vehicleState Reference to a VehicleState
    ////////////////////////////////////////////////////////////
    void checkWaitTime(VehicleState &vehicleState);

    ////////////////////////////////////////////////////////////
    ///  @brief See if there is a car waiting in any lane.
    ///  
    ////////////////////////////////////////////////////////////
    void checkIfCarsAreWaiting();

    ////////////////////////////////////////////////////////////
    ///  @brief Update TrafficSignals lane and state.
    ///  
    ///  @param lane The lane to set
    ///  @param state The state to set
    ////////////////////////////////////////////////////////////
    void setLaneSignal(Lane lane, SignalState state);

    ////////////////////////////////////////////////////////////
    ///  @brief Converts a TrafficLightPattern to a string.
    ///  
    ///  @param pattern The TrafficLightPattern to convert
    ///  @return std::string The string version of the TrafficLightPattern
    ////////////////////////////////////////////////////////////
    std::string lightPatternToString(TrafficLightPattern pattern);

    ////////////////////////////////////////////////////////////
    ///  @brief Converts a Lane to a string.
    ///  
    ///  @param lane The Lane to convert
    ///  @return std::string The string version of the Lane
    ////////////////////////////////////////////////////////////
    std::string laneToString(Lane lane);

    ////////////////////////////////////////////////////////////
    ///  @brief Initialize the default TrafficLightStates.
    ///  
    ////////////////////////////////////////////////////////////
    void populateLightStates();

    ////////////////////////////////////////////////////////////
    ///  @brief Initialize the default VehicleStates.
    ///  
    ////////////////////////////////////////////////////////////
    void populateVehicleStates();

    ////////////////////////////////////////////////////////////
    ///  @brief Initialize a Lane to opposing lanes mapping.
    ///  
    ////////////////////////////////////////////////////////////
    void makeOpposingLanesMap();

    ////////////////////////////////////////////////////////////
    ///  @brief Initialize a TrafficLightPattern to Lane mapping.
    ///  
    ////////////////////////////////////////////////////////////
    void makePatternToLaneMap();

    // Members
    const Clock &clock_; ///< Clock reference from Simulator
    const VehicleSensors &sensors_; ///< Reference to signal for each lane
    TrafficSignals signals_; ///< Signals for each lane
    bool appState_; ///< Is this app in a good state or not
    bool carsAwaiting_; ///< Are there cars waiting at red lights
    IClock::Time maxWaitTime_; ///< Config driven value for maxWaitTime at red light
    std::array<TrafficLightState, TrafficLightPattern::NUM_PATTERNS> lightStates_; ///< States of traffic light for each pattern
    std::array<VehicleState, Lane::COUNT> vehicleStates_; ///< States of vehicle for each lane
    std::map<Lane, std::array<Lane, Lane::COUNT>> laneToOpposingLaneMap_; ///< Map of Lanes to it's opposing lanes
    std::map<TrafficLightPattern, std::array<Lane, Lane::COUNT>> patternToLaneMap_; ///< Map of patterns to it's lanes
};

#endif // INCLUDE_TRAFFICLIGHTCONTROLLERAPP_H_
