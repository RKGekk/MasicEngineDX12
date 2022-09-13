#include "game_timer.h"

#include <ratio>

GameTimer::GameTimer() : GameTimerDelta(ZERO_DURATION, ZERO_DURATION), m_paused_time(ZERO_DURATION) {
    GameTimePoint start_time = GameClock::now();
    m_curent_time = start_time;
    m_base_time = start_time;
    m_stop_time = start_time;
    m_prev_time = start_time;

    m_stopped = false;
}

void GameTimer::Tick() {
    if (m_stopped) {
        m_delta_time_duration = ZERO_DURATION;
        return;
    }

    m_curent_time = GameClock::now();
    m_delta_time_duration = m_curent_time - m_prev_time;
    m_prev_time = m_curent_time;

    if (m_stopped) {
        m_total_time = m_stop_time - m_base_time - m_paused_time;
    }
    else {
        m_total_time = m_curent_time - m_base_time - m_paused_time;
    }

    if (m_delta_time_duration < ZERO_DURATION) {
        m_delta_time_duration = ZERO_DURATION;
    }
}

void GameTimer::Start() {
    GameTimePoint start_time = GameClock::now();

    if (m_stopped) {
        m_paused_time += (start_time - m_stop_time);

        m_prev_time = start_time;
        m_stop_time = start_time;
        m_stopped = false;
    }
    else {
        Reset();
    }
}

void GameTimer::Stop() {
    if (!m_stopped) {
        m_stop_time = GameClock::now();
        m_stopped = true;
    }
}

void GameTimer::Reset() {
    GameTimePoint start_time = GameClock::now();
    m_base_time = start_time;
    m_prev_time = start_time;
    m_stop_time = start_time;
    m_stopped = false;

    m_delta_time_duration = ZERO_DURATION;
    m_total_time = ZERO_DURATION;
    m_paused_time = ZERO_DURATION;
}

GameTimerDelta::GameTimerDelta() {
    m_delta_time_duration = {};
    m_total_time = {};
}

GameTimerDelta::GameTimerDelta(const GameClockDuration& delta_time, const GameClockDuration& total_time) : m_delta_time_duration(delta_time), m_total_time(total_time) {}

double GameTimerDelta::GetDeltaNanoseconds() const {
    return std::chrono::duration<double, std::nano>(m_delta_time_duration).count();
}
double GameTimerDelta::GetDeltaMicroseconds() const {
    return std::chrono::duration<double, std::micro>(m_delta_time_duration).count();
}

double GameTimerDelta::GetDeltaMilliseconds() const {
    return std::chrono::duration<double, std::milli>(m_delta_time_duration).count();
}

double GameTimerDelta::GetDeltaSeconds() const {
    return std::chrono::duration<double>(m_delta_time_duration).count();
}

float GameTimerDelta::fGetDeltaNanoseconds() const {
    return std::chrono::duration<float, std::nano>(m_delta_time_duration).count();
}
float GameTimerDelta::fGetDeltaMicroseconds() const {
    return std::chrono::duration<float, std::micro>(m_delta_time_duration).count();
}

float GameTimerDelta::fGetDeltaMilliseconds() const {
    return std::chrono::duration<float, std::milli>(m_delta_time_duration).count();
}

float GameTimerDelta::fGetDeltaSeconds() const {
    return std::chrono::duration<float>(m_delta_time_duration).count();
}

const GameClockDuration& GameTimerDelta::GetDeltaDuration() const {
    return m_delta_time_duration;
}

void GameTimerDelta::AddDeltaDuration(const GameClockDuration& delta) {
    m_total_time += delta;
    m_delta_time_duration = delta;
}

void GameTimerDelta::AddDeltaDuration(const GameTimerDelta& delta) {
    GameClockDuration d = delta.GetDeltaDuration();
    m_total_time += d;
    m_delta_time_duration = d;
}

double GameTimerDelta::GetTotalNanoseconds() const {
    return std::chrono::duration<double, std::nano>(m_total_time).count();
}

double GameTimerDelta::GetTotalMicroseconds() const {
    return std::chrono::duration<double, std::micro>(m_total_time).count();
}

double GameTimerDelta::GetTotalMilliSeconds() const {
    return std::chrono::duration<double, std::milli>(m_total_time).count();
}

double GameTimerDelta::GetTotalSeconds() const {
    return std::chrono::duration<double>(m_total_time).count();
}

float GameTimerDelta::fGetTotalNanoseconds() const {
    return std::chrono::duration<float, std::nano>(m_total_time).count();
}

float GameTimerDelta::fGetTotalMicroseconds() const {
    return std::chrono::duration<float, std::micro>(m_total_time).count();
}

float GameTimerDelta::fGetTotalMilliSeconds() const {
    return std::chrono::duration<float, std::milli>(m_total_time).count();
}

float GameTimerDelta::fGetTotalSeconds() const {
    return std::chrono::duration<float>(m_total_time).count();
}

const GameClockDuration& GameTimerDelta::GetTotalDuration() const {
    return m_total_time;
}
