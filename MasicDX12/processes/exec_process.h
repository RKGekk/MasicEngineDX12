#pragma once

#pragma once

#include <iostream>
#include <functional>
#include <utility>

#include "process.h"

class ExecProcess : public Process {
public:
	ExecProcess(std::function<bool()> fn);

protected:
	virtual void VOnUpdate(const GameTimerDelta& delta) override;

private:
	std::function<bool()> m_fn;
};