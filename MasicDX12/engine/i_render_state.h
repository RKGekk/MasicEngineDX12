#pragma once

#include <string>

class IRenderState {
public:
	virtual std::string VToString() = 0;
};