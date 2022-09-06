#include "delay_process.h"

DelayProcess::DelayProcess(GameClockDuration delay, std::function<bool(const GameTimerDelta& delta, float)> fn) : m_count_to(delay), m_fn(std::move(fn)) {
	m_count_to_inv = (1.0f / std::chrono::duration<float>(delay).count());
}

void DelayProcess::VOnUpdate(const GameTimerDelta& delta) {
	m_total_time.AddDeltaDuration(delta.GetDeltaDuration());
	GameClockDuration my_total_time = m_total_time.GetTotalDuration();
	float tt = std::chrono::duration<float>(my_total_time).count();
	float n = std::clamp(tt * m_count_to_inv, 0.0f, 1.0f);
	if (!m_fn(m_total_time, n)) {
		Fail();
	};
	if (my_total_time >= m_count_to) {
		Succeed();
	}
}