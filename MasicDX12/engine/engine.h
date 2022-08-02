#pragma once

#include <memory>

#include <DirectXColors.h>
#include <DirectXPackedVector.h>

#include "../application_options.h"
#include "../render_window_config.h"
#include "../window_surface.h"
#include "../tools/game_timer.h"
#include "base_engine_logic.h"
//#include "i_screen_element.h"
#include "renderer_enum.h"
#include "../graphics/i_renderer.h"
#include "../events/event_manager.h"
//#include "human_view.h"
#include "../graphics/adapter_reader.h"

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

	ApplicationOptions& GetConfig();
	const WindowSurface& GetRenderWindow();
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

	ApplicationOptions m_options;
	WindowSurface m_render_window;
	std::shared_ptr<AdapterReader> m_adapter_reader;
	AdapterData::AdapterDataPtr m_adapter;



	std::unique_ptr<IRenderer> m_renderer;
	std::unique_ptr<BaseEngineLogic> m_game;

	static std::unique_ptr<Engine> m_pEngine;
};