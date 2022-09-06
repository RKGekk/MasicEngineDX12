#pragma once

#include <iostream>
#include <functional>
#include <utility>
#include <algorithm>

#include "process.h"
#include "../tools/game_timer.h"

class DelayProcess : public Process {
public:
	DelayProcess(GameClockDuration delay, std::function<bool(const GameTimerDelta& delta, float n)> fn);

protected:
	virtual void VOnUpdate(const GameTimerDelta& delta) override;

private:
	GameTimerDelta m_total_time;
	GameClockDuration m_count_to;
	float m_count_to_inv;
	std::function<bool(const GameTimerDelta&, float)> m_fn;
};