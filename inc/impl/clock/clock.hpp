#ifndef INCLUDE_CLOCK_H_
#define INCLUDE_CLOCK_H_

#include "interfaces/clock/IClock.hpp"

////////////////////////////////////////////////////////////
///  @brief A simple clock abstraction for simulating time.
///     To keep things extra simple, all timestamps can be treated as seconds.
///  
////////////////////////////////////////////////////////////
class Clock : public IClock
{
public:
    ////////////////////////////////////////////////////////////
    ///  @brief Construct a new Clock object
    ///  
    ////////////////////////////////////////////////////////////
    Clock(void) :
        now_(0)
    { }

    ////////////////////////////////////////////////////////////
    ///  @brief Get the current timestamp
    ///  
    ///  @return Time The time right now
    ////////////////////////////////////////////////////////////
    inline Time now(void) const override
    {
        return now_;
    }

    ////////////////////////////////////////////////////////////
    ///  @brief Set the current timestamp
    ///  
    ///  @param now Set the time to now
    ////////////////////////////////////////////////////////////
    inline void set(Time now) override
    {
        now_ = now;
    }

    ////////////////////////////////////////////////////////////
    ///  @brief Advance the current timestamp
    ///  
    ///  @param delta Amount to advance time by
    ////////////////////////////////////////////////////////////
    inline void advance(Time delta) override
    {
        now_ += delta;
    }

    ////////////////////////////////////////////////////////////
    ///  @brief Calculate the time elapsed (or until) an event
    ///  
    ///  @param then Some time greater than the current time
    ///  @return Time The difference between now and then
    ////////////////////////////////////////////////////////////
    inline Time elapsed(Time then) const override
    {
        return now_ - then;
    }

private:
    Time now_; ///< current timestamp
};

#endif // INCLUDE_CLOCK_H_
