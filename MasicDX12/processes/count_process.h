#pragma once

#include <iostream>
#include <functional>
#include <utility>

#include "process.h"

class CountProcess : public Process {
public:
	CountProcess(unsigned int count_to, std::function<void(unsigned int)> fn);

protected:
	virtual void VOnUpdate(float deltaMs) override;

private:
	unsigned int m_count = 0u;
	unsigned int m_count_to = 0u;
	std::function<void(unsigned int)> m_fn;
};