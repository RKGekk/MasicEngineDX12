#pragma once

#include <iostream>
#include <functional>
#include <utility>

#include "process.h"
#include "../tools/game_timer.h"

class CountProcess : public Process {
public:
	CountProcess(unsigned int count_to, std::function<void(unsigned int)> fn);

protected:
	virtual void VOnUpdate(const GameTimerDelta& delta) override;

private:
	unsigned int m_count = 0u;
	unsigned int m_count_to = 0u;
	std::function<void(unsigned int)> m_fn;
};