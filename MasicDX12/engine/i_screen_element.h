#pragma once

#include <list>
#include <memory>

#include <Windows.h>

#include "../tools/game_timer.h"

class CommandList;
class IScreenElement;
typedef std::list<std::shared_ptr<IScreenElement>> ScreenElementList;

class IScreenElement {
public:
	virtual ~IScreenElement() {};

	virtual HRESULT VOnRestore() = 0;
	virtual HRESULT VOnLostDevice() = 0;
	virtual HRESULT VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList>) = 0;
	virtual void VOnUpdate(const GameTimerDelta& delta) = 0;

	virtual int VGetZOrder() const = 0;
	virtual void VSetZOrder(int const zOrder) = 0;
	virtual bool VIsVisible() const = 0;
	virtual void VSetVisible(bool visible) = 0;

	virtual LRESULT CALLBACK VOnMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual bool const operator <(IScreenElement const& other) { return VGetZOrder() < other.VGetZOrder(); }
};