#pragma once

#include <iostream>
#include <functional>
#include <utility>
#include <algorithm>

#include "process.h"

class DelayProcess : public Process {
public:
	DelayProcess(float delay_in_seconds, std::function<bool(float dt, float tt, float n)> fn);

protected:
	virtual void VOnUpdate(float deltaMs) override;

private:
	float m_total_time = 0.0f;
	float m_count_to = 0.0f;
	float m_count_to_inv = 0.0f;
	std::function<bool(float, float, float)> m_fn;
};