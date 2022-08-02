#pragma once

#include <chrono>

using GameClock = std::chrono::steady_clock;
using GameClockDuration = GameClock::duration;
using GameTimePoint = GameClock::time_point;

const GameClockDuration ZERO_DURATION = GameClockDuration(0);

class GameTimerDelta {
public:
    GameTimerDelta(GameClockDuration delta_time, GameClockDuration total_time);

    double GetDeltaNanoseconds() const;
    double GetDeltaMicroseconds() const;
    double GetDeltaMilliseconds() const;
    double GetDeltaSeconds() const;

    float fGetDeltaNanoseconds() const;
    float fGetDeltaMicroseconds() const;
    float fGetDeltaMilliseconds() const;
    float fGetDeltaSeconds() const;

    GameClockDuration GetDeltaDuration() const;

    double GetTotalNanoseconds() const;
    double GetTotalMicroseconds() const;
    double GetTotalMilliSeconds() const;
    double GetTotalSeconds() const;

    float fGetTotalNanoseconds() const;
    float fGetTotalMicroseconds() const;
    float fGetTotalMilliSeconds() const;
    float fGetTotalSeconds() const;

    GameClockDuration GetTotalDuration() const;

protected:
    GameClockDuration m_delta_time_duration;
    GameClockDuration m_total_time;
};

class GameTimer : public GameTimerDelta {
public:
    GameTimer();

    void Tick();
    void Start();
    void Stop();
    void Reset();

private:
    GameTimePoint m_curent_time;
    GameTimePoint m_base_time;
    GameClockDuration m_paused_time;
    GameTimePoint m_stop_time;
    GameTimePoint m_prev_time;

    bool m_stopped;
};