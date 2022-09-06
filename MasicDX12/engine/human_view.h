#pragma once

#include <memory>
#include <queue>
#include <vector>

#include <DirectXMath.h>

#include "../graphics/imgui/imgui.h"
#include "../graphics/imgui/imgui_impl_win32.h"
#include "../graphics/imgui/imgui_impl_dx12.h"

#include "i_engine_view.h"
#include "../processes/process_manager.h"
#include "base_engine_state.h"
#include "i_screen_element.h"
#include "../graphics/i_renderer.h"
#include "i_pointer_handler.h"
#include "i_keyboard_handler.h"
#include "../nodes/camera_node.h"
#include "screen_element_scene.h"

class HumanView : public IEngineView {
	friend class Application;
	friend class Engine;

	static const std::string g_Name;

public:
	HumanView();
	virtual ~HumanView();

	bool LoadGame(const pugi::xml_node& pLevelData);

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnLostDevice() override;

	virtual void VOnRender(const GameTimerDelta& delta) override;
	virtual void VOnUpdate(const GameTimerDelta& delta) override;

	virtual EngineViewType VGetType() override;
	virtual EngineViewId VGetId() const override;

	virtual void VOnAttach(EngineViewId vid, ActorId aid) override;
	virtual LRESULT CALLBACK VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) override;
	
	virtual void VPushElement(std::shared_ptr<IScreenElement> pElement);
	virtual void VRemoveElement(std::shared_ptr<IScreenElement> pElement);

	virtual void VActivateScene(bool is_active);
	virtual void VCanDraw(bool is_can_draw);

	void TogglePause(bool active);
	void HandleGameState(BaseEngineState newState);

	virtual void VSetControlledActor(ActorId actorId);
	virtual std::shared_ptr<CameraNode> VGetCamera();
	virtual void VSetCameraByName(std::string camera_name);

	virtual const std::string& VGetName() override;

	void GameStateDelegate(IEventDataPtr pEventData);

protected:
	virtual bool VLoadGameDelegate(const pugi::xml_node& pLevelData);

	EngineViewId m_view_id;
	ActorId m_actor_id;
	BaseEngineState m_base_game_state;

	GameClockDuration m_current_tick;
	GameClockDuration m_last_draw;
	bool m_run_full_speed;
	bool m_can_draw = true;

	std::unique_ptr<ProcessManager> m_process_manager;
	ScreenElementList m_screen_elements;
	std::shared_ptr<ScreenElementScene> m_scene;
	std::weak_ptr<CameraNode> m_camera;

	float m_pointer_radius;
	std::vector<std::shared_ptr<IPointerHandler>> m_pointer_handlers;
	std::vector<std::shared_ptr<IKeyboardHandler>> m_keyboard_handlers;

	virtual void VRenderText();

private:
	void RegisterAllDelegates();
	void RemoveAllDelegates();
};