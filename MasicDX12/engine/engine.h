#pragma once

#include <memory>

#include <DirectXColors.h>
#include <DirectXPackedVector.h>

#include "render_window.h"
#include "../tools/game_timer.h"
#include "base_engine_logic.h"
//#include "i_screen_element.h"
#include "renderer_enum.h"
#include "../graphics/i_renderer.h"
#include "../events/event_manager.h"
//#include "human_view.h"

class Engine {
public:
	virtual ~Engine();

	bool Initialize(const RenderWindowConfig& cfg);
	void Run();
	void AbortGame();
	bool IsRunning();

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnDisplayChange(int colorDepth, int width, int height);
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnClose();
	LRESULT OnAltEnter();
	LRESULT OnNcCreate(LPCREATESTRUCT cs);

	EngineOptions& GetConfig();
	const RenderWindow& GetRenderWindow();
	static Renderer GetRendererImpl();

	BaseEngineLogic* GetGameLogic();
	IRenderer* GetRenderer();
	static Engine* GetEngine();
	//std::shared_ptr<HumanView> GetHumanView();
	//std::shared_ptr<HumanView> GetHumanViewByName(std::string name);
	GameTimer& GetTimer();

	bool ProcessMessages();
	bool Update();
	void RenderFrame();
	//virtual bool VLoadGame();
	//int Modal(std::shared_ptr<IScreenElement> pModalScreen, int defaultAnswer);

protected:
	Engine();
	//virtual std::unique_ptr<BaseEngineLogic> VCreateGameAndView();
	virtual void VRegisterEvents();
	virtual void RegisterAllDelegates();

	void OSMessageDelegate(IEventDataPtr pEventData);

private:
	bool m_is_running = false;
	bool m_is_editor_running = false;
	bool m_is_quit_requested = false;
	bool m_is_quitting = false;
	int m_has_modal_dialog = false;

	EngineOptions m_options;
	RenderWindow m_render_window;
	GameTimer m_timer;

	std::unique_ptr<EventManager> m_event_manager;
	std::unique_ptr<IRenderer> m_renderer;

	std::unique_ptr<BaseEngineLogic> m_game;

	static std::unique_ptr<Engine> m_pEngine;
};