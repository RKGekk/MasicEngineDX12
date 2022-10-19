#pragma once

#include <memory>
#include <queue>
#include <vector>

#include <DirectXMath.h>

#include "i_engine_view.h"
#include "../processes/process_manager.h"
#include "base_engine_state.h"
#include "i_screen_element.h"
#include "../graphics/i_renderer.h"
#include "i_pointer_handler.h"
#include "i_keyboard_handler.h"
#include "../nodes/camera_node.h"
#include "../nodes/basic_camera_node.h"
#include "screen_element_scene.h"
#include "actor_menu_ui.h"
#include "movement_controller.h"

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

	virtual void VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList> command_list) override;
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

	virtual void VSetControlledActor(std::shared_ptr<Actor> actor);
	virtual std::shared_ptr<BasicCameraNode> VGetCamera();
	virtual std::shared_ptr<Scene> VGetScene();
	virtual void VSetCameraByName(std::string camera_name);

	virtual const std::string& VGetName() override;

	void GameStateDelegate(IEventDataPtr pEventData);
	void NewSceneNodeComponentDelegate(IEventDataPtr pEventData);
	void DestroySceneNodeComponentDelegate(IEventDataPtr pEventData);

protected:
	virtual bool VLoadGameDelegate(const pugi::xml_node& pLevelData);
	virtual void VRenderText();

	EngineViewId m_view_id;
	std::weak_ptr<Actor> m_actor;
	BaseEngineState m_base_game_state;

	GameClockDuration m_current_tick;
	GameClockDuration m_last_draw;
	bool m_run_full_speed;
	bool m_can_draw = true;

	std::shared_ptr<ProcessManager> m_process_manager;
	ScreenElementList m_screen_elements;
	std::shared_ptr<ScreenElementScene> m_scene;
	std::weak_ptr<BasicCameraNode> m_camera;

	float m_pointer_radius;
	std::vector<std::shared_ptr<IPointerHandler>> m_pointer_handlers;
	std::vector<std::shared_ptr<IKeyboardHandler>> m_keyboard_handlers;

	bool m_bShow_ui;
	bool m_bShow_debug_ui;
	std::wstring m_gameplay_text;
	std::shared_ptr<ActorMenuUI> m_actor_menu_ui;

	std::shared_ptr<MovementController> m_pFree_camera_controller;
	std::weak_ptr<Actor> m_pTeapot;

	HWND m_hwnd;
	std::shared_ptr<GUI> m_gui;

private:
	void RegisterAllDelegates();
	void RemoveAllDelegates();
};