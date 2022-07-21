#pragma once

#include <string>

class IPointerHandler {
public:
	virtual bool VOnPointerMove(int x, int y, const int radius) = 0;
	virtual bool VOnPointerButtonDown(int x, int y, const int radius, const std::string& buttonName) = 0;
	virtual bool VOnPointerButtonUp(int x, int y, const int radius, const std::string& buttonName) = 0;
};