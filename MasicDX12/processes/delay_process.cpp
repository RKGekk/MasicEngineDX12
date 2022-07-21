#include "delay_process.h"

DelayProcess::DelayProcess(float delay_in_seconds, std::function<bool(float, float, float)> fn) : m_count_to(delay_in_seconds), m_count_to_inv(1.0f / delay_in_seconds), m_fn(std::move(fn)) {}

void DelayProcess::VOnUpdate(float deltaMs) {
	m_total_time += deltaMs;
	float n = std::clamp(m_total_time * m_count_to_inv, 0.0f, 1.0f);
	if (!m_fn(deltaMs, m_total_time, n)) {
		Fail();
	};
	if (m_total_time >= m_count_to) {
		Succeed();
	}
}