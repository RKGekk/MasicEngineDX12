#include "game_timer.h"

#include <ratio>

GameTimer::GameTimer() : m_delta_time_duration(ZERO_DURATION), m_total_time(ZERO_DURATION), m_paused_time(ZERO_DURATION) {
    gameTimePoint start_time = gameClock::now();
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

    m_curent_time = gameClock::now();
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
    gameTimePoint start_time = gameClock::now();

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
        m_stop_time = gameClock::now();
        m_stopped = true;
    }
}

void GameTimer::Reset() {
    gameTimePoint start_time = gameClock::now();
    m_base_time = start_time;
    m_prev_time = start_time;
    m_stop_time = start_time;
    m_stopped = false;

    m_delta_time_duration = ZERO_DURATION;
    m_total_time = ZERO_DURATION;
    m_paused_time = ZERO_DURATION;
}

double GameTimer::GetDeltaNanoseconds() const {
    return std::chrono::duration<double, std::nano>(m_delta_time_duration).count();
}
double GameTimer::GetDeltaMicroseconds() const {
    return std::chrono::duration<double, std::micro>(m_delta_time_duration).count();
}

double GameTimer::GetDeltaMilliseconds() const {
    return std::chrono::duration<double, std::milli>(m_delta_time_duration).count();
}

double GameTimer::GetDeltaSeconds() const {
    return std::chrono::duration<double>(m_delta_time_duration).count();
}

float GameTimer::fGetDeltaNanoseconds() const {
    return std::chrono::duration<float, std::nano>(m_delta_time_duration).count();
}
float GameTimer::fGetDeltaMicroseconds() const {
    return std::chrono::duration<float, std::micro>(m_delta_time_duration).count();
}

float GameTimer::fGetDeltaMilliseconds() const {
    return std::chrono::duration<float, std::milli>(m_delta_time_duration).count();
}

float GameTimer::fGetDeltaSeconds() const {
    return std::chrono::duration<float>(m_delta_time_duration).count();
}

GameTimer::gameClockDuration GameTimer::GetDeltaDuration() const {
    return m_delta_time_duration;
}

double GameTimer::GetTotalNanoseconds() const {
    return std::chrono::duration<double, std::nano>(m_total_time).count();
}

double GameTimer::GetTotalMicroseconds() const {
    return std::chrono::duration<double, std::micro>(m_total_time).count();
}

double GameTimer::GetTotalMilliSeconds() const {
    return std::chrono::duration<double, std::milli>(m_total_time).count();
}

double GameTimer::GetTotalSeconds() const {
    return std::chrono::duration<double>(m_total_time).count();
}

float GameTimer::fGetTotalNanoseconds() const {
    return std::chrono::duration<float, std::nano>(m_total_time).count();
}

float GameTimer::fGetTotalMicroseconds() const {
    return std::chrono::duration<float, std::micro>(m_total_time).count();
}

float GameTimer::fGetTotalMilliSeconds() const {
    return std::chrono::duration<float, std::milli>(m_total_time).count();
}

float GameTimer::fGetTotalSeconds() const {
    return std::chrono::duration<float>(m_total_time).count();
}

GameTimer::gameClockDuration GameTimer::GetTotalDuration() const {
    return m_total_time;
}
