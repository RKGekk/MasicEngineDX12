#pragma once

#include <memory>
#include <string>

#include <Windows.h>

#include "engine_view_type.h"
#include "../actors/actor.h"

class IEngineView;

typedef unsigned int EngineViewId;
typedef std::list<std::shared_ptr<IEngineView>> GameViewList;

class IEngineView {
public:
	virtual HRESULT VOnRestore() = 0;
	virtual void VOnRender(double fTime, float fElapsedTime) = 0;
	virtual HRESULT VOnLostDevice() = 0;
	virtual EngineViewType VGetType() = 0;
	virtual const std::string& VGetName() = 0;
	virtual EngineViewId VGetId() const = 0;
	virtual void VOnAttach(EngineViewId vid, ActorId aid) = 0;

	virtual LRESULT CALLBACK VOnMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual void VOnUpdate(float deltaMs) = 0;

	virtual ~IEngineView() {};
};