#ifndef INCLUDE_SIMULATOR_H_
#define INCLUDE_SIMULATOR_H_

#include "interfaces/simulator/ISimulator.hpp"

#include "impl/clock/clock.hpp"

#include <array>
#include <iostream>
#include <vector>

////////////////////////////////////////////////////////////
///  @brief Each lane is identified by it's enter and exit directions.
/// 
/// Here, enter and exit refere to the direction of travel as the vehicles
/// enters or exits the intersection.
///  
////////////////////////////////////////////////////////////
enum Lane
{
    N_N, ///< north going straight through
    N_W, ///< north turning west
    S_S, ///< south going straight through
    S_E, ///< south turning east
    E_E, ///< east going straight through
    E_N, ///< east turning north
    W_W, ///< west going straight through
    W_S, ///< west turning south
    COUNT,
    PLACEHOLDER
};

enum class SensorState
{
    SET,  ///< Car is present
    CLEAR ///< No car is present
};

/// There is one vehicle sensor per lane
using VehicleSensors = std::array<SensorState, Lane::COUNT>;

enum class SignalState
{
    RED,
    YELLOW,
    GREEN
};

/// There is one traffic signal per lane
using TrafficSignals = std::array<SignalState, Lane::COUNT>;

/// Describes the simulator state for the timespan [start, end).
struct SimulationTimeslice
{
    Clock::Time start;      ///< start time, inclusive
    Clock::Time end;        ///< end time, exclusive
    VehicleSensors sensors; ///< state of each vehicle sensor
};

////////////////////////////////////////////////////////////
///  @brief A "scenario" is a list of simulation states that is replayed by the
///  simulator.
///  
///  Ideally, scenarios should be the input to a feedback loop running within the
///  simulator, so you could do things like trigger simulation state changes from
///  events emitted by the controller. As implemented, they are replayed blindly,
///  ie. providing open-loop simulation.
///  
////////////////////////////////////////////////////////////
using Scenario = std::vector<SimulationTimeslice>;

////////////////////////////////////////////////////////////
///  @brief A simulator replays data from a given scenario.
///
///     To advance time in the simulator, call #advance(). The simulator reads
///     vehicle sensor data from the scenario when the simulation time is advanced.
///     
///     The current simulation time can be retrieved via #clock(). The vehicle
///     sensor data for the current timestamp can be retrieved with #sensors().
///     
///     The simulator also maintains the list of traffic signals for each lane. To
///     update the list, use #update_lane_signals();
///  
////////////////////////////////////////////////////////////
class Simulator : public ISimulator
{
public:
    Simulator(const Scenario &scenario) : 
        scenario_(scenario),
        done_(false)
    { 
        for (unsigned lane = 0; lane < Lane::COUNT; ++lane)
        {
            sensors_[lane] = SensorState::CLEAR;
            signals_[lane] = SignalState::RED;
        }
        
        update_simulation(); // seed the simulator
    }

    /// Banner text
    static const std::string BANNER;
    
    /// Print the state of the simulator to the given outptut stream
    friend std::ostream& operator<<(std::ostream &os, const Simulator &simulator);

    ////////////////////////////////////////////////////////////
    ///  @brief Get the simulated clock
    ///  
    ///  @return const Clock& 
    ////////////////////////////////////////////////////////////
    inline const Clock& clock(void) const
    {
        return clock_;
    }

    ////////////////////////////////////////////////////////////
    ///  @brief Get the simulated sensors's state at the current timestamp.
    ///
    ///  Vehicle sensors are updated from the loaded scenario during the call to
    ///  #advance().
    ///  
    ///  @return const VehicleSensors& 
    ////////////////////////////////////////////////////////////
    inline const VehicleSensors& sensors(void) const
    {
        return sensors_;
    }

    ////////////////////////////////////////////////////////////
    ///  @brief Users should call this to update the traffic lights in the simulated intersection
    ///  
    ///  @param signals Reference to a TrafficSignals
    ////////////////////////////////////////////////////////////
    inline void update_lane_signals(const TrafficSignals &signals)
    {
        signals_ = signals;
    }

    ////////////////////////////////////////////////////////////
    ///  @brief Returns true iff the scenario has been completed
    ///  
    ///  @return true If scenario is completed
    ///  @return false If scenario is still processing
    ////////////////////////////////////////////////////////////
    inline bool done(void) const
    {
        return done_;
    }

    ////////////////////////////////////////////////////////////
    ///  @brief Users should call this to advance the simulation time by the given delta.
    ///
    ///  There are no restrictions placed on the size of the time delta to
    ///  advance by. However, care should be taken that a suitable value is used
    ///  which does not skip time-slices in the loaded scenario.
    ///  
    ///  @param delta Advance the simulator by this delta
    ////////////////////////////////////////////////////////////
    inline void advance(Clock::Time delta)
    {
        clock_.advance(delta);
        update_simulation();
    }

private:
    ////////////////////////////////////////////////////////////
    ///  @brief Updates the simulator state.
    ///
    ///  The simulation state is read from the loaded scenario for the current
    ///  timestamp.
    ///  
    ////////////////////////////////////////////////////////////
    void update_simulation(void);

    const Scenario scenario_; ///< copy of loaded scenario
    Clock clock_;             ///< global simulation clock
    bool done_;               ///< true iff scenario completed
    VehicleSensors sensors_;  ///< sensor state for given timestamp
    TrafficSignals signals_;  ///< simulated traffic signal output
};

#endif // INCLUDE_SIMULATOR_H_
