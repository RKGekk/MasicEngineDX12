#pragma once

#include <memory>
#include <string>

#include <Windows.h>

#include "engine_view_type.h"
#include "../actors/actor.h"
#include "../tools/game_timer.h"

class IEngineView;
class CommandList;

typedef unsigned int EngineViewId;
typedef std::list<std::shared_ptr<IEngineView>> GameViewList;

class IEngineView {
public:
	virtual HRESULT VOnRestore() = 0;
	virtual HRESULT VOnLostDevice() = 0;

	virtual void VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList>) = 0;
	virtual void VOnUpdate(const GameTimerDelta& delta) = 0;
	
	virtual EngineViewType VGetType() = 0;
	virtual const std::string& VGetName() = 0;
	virtual EngineViewId VGetId() const = 0;

	virtual void VOnAttach(EngineViewId vid, ActorId aid) = 0;

	virtual LRESULT CALLBACK VOnMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	virtual ~IEngineView() {};
};