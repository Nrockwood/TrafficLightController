#include "impl/simulator/simulator.hpp"
#include "impl/app/TrafficLightControllerApp.hpp"

using SS = SensorState;

static constexpr Clock::Time TIME_STEP = 10; ///< advance simulator by 10s for each step

/// At T+0, non-stop traffic in all directions. continues for 5 minutes
static const Scenario SCENARIO_1 = 
{   //            N-N        N-W        S-S        S-E        E-E        E-N        W-W        W-S
    { 0,  300,  { SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET   }}
};

/// at T+0, there is  N-W and S-E traffic
/// at T+10, an infinite line of vehicles pulls up to the N-N sensor
/// at T+20, all N-W and S-E traffic stops
static const Scenario SCENARIO_2 = 
{   //             N-N        N-W        S-S        S-E        E-E        E-N        W-W        W-S
    { 0,   10,   { SS::CLEAR, SS::SET,   SS::CLEAR, SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 10,  20,   { SS::SET,   SS::SET,   SS::CLEAR, SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 20,  300,  { SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }}
};

static const Scenario SCENARIO_3 =
{   //             N-N        N-W        S-S        S-E        E-E        E-N        W-W        W-S
    { 0,   300,  { SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET   }},
    { 300, 310,  { SS::CLEAR, SS::SET,   SS::CLEAR, SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 310, 330,  { SS::SET,   SS::SET,   SS::CLEAR, SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 330, 600,  { SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }}
};

static const Scenario SCENARIO_4 =
{   //             N-N        N-W        S-S        S-E        E-E        E-N        W-W        W-S
    { 0,   300,  { SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET   }},
    { 300, 310,  { SS::CLEAR, SS::SET,   SS::CLEAR, SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 310, 330,  { SS::SET,   SS::SET,   SS::CLEAR, SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 330, 600,  { SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 600, 900,  { SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET   }}
};

/// Ideally I would move this maxWaitTime value into a config file
/// and a ConfigClass would be passed into TrafficLightControllerApp::ctor()
static IClock::Time maxWaitTime = 40;

void runScenarios(Scenario scenario)
{
    Simulator simulator(scenario);
    auto &clock = simulator.clock();
    auto &sensors = simulator.sensors();
    TrafficLightControllerApp tlcApp(clock, sensors, maxWaitTime);
    tlcApp.initApp();

    std::cout << simulator.BANNER << std::endl;

    for(;;)
    {
        if (simulator.done())
        {
            break;
        }

        tlcApp.run();
        auto &signals = tlcApp.getSignals();
        simulator.update_lane_signals(signals);

        std::cout << simulator << std::endl;
        simulator.advance(TIME_STEP);
    }
}

int main
(
    int argc, 
    char const *argv[]
)
{
    runScenarios(SCENARIO_1);
    runScenarios(SCENARIO_2);
    runScenarios(SCENARIO_3);
    runScenarios(SCENARIO_4);
}
