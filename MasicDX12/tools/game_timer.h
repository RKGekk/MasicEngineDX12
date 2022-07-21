#pragma once

#include <chrono>

class GameTimer {
public:
    using gameClock = std::chrono::steady_clock;
    using gameClockDuration = gameClock::duration;
    using gameTimePoint = gameClock::time_point;

    const gameClockDuration ZERO_DURATION = gameClockDuration(0);

    GameTimer();

    void Tick();
    void Start();
    void Stop();
    void Reset();

    double GetDeltaNanoseconds() const;
    double GetDeltaMicroseconds() const;
    double GetDeltaMilliseconds() const;
    double GetDeltaSeconds() const;

    float fGetDeltaNanoseconds() const;
    float fGetDeltaMicroseconds() const;
    float fGetDeltaMilliseconds() const;
    float fGetDeltaSeconds() const;

    gameClockDuration GetDeltaDuration() const;

    double GetTotalNanoseconds() const;
    double GetTotalMicroseconds() const;
    double GetTotalMilliSeconds() const;
    double GetTotalSeconds() const;

    float fGetTotalNanoseconds() const;
    float fGetTotalMicroseconds() const;
    float fGetTotalMilliSeconds() const;
    float fGetTotalSeconds() const;

    gameClockDuration GetTotalDuration() const;

private:
    gameTimePoint m_curent_time;
    gameTimePoint m_base_time;
    gameClockDuration m_paused_time;
    gameTimePoint m_stop_time;
    gameTimePoint m_prev_time;

    gameClockDuration m_delta_time_duration;
    gameClockDuration m_total_time;

    bool m_stopped;
};