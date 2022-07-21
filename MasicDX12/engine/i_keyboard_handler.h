#pragma once

class IKeyboardHandler {
public:
	virtual bool VOnKeyDown(unsigned char c) = 0;
	virtual bool VOnKeyUp(unsigned char c) = 0;
};