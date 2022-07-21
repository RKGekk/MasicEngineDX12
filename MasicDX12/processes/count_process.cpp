#include "count_process.h"

CountProcess::CountProcess(unsigned int count_to, std::function<void(unsigned int)> fn) : m_count_to(count_to), m_fn(std::move(fn)) {}

void CountProcess::VOnUpdate(float deltaMs) {
	++m_count;
	m_fn(m_count);
	if (m_count == m_count_to) { Succeed(); }
}