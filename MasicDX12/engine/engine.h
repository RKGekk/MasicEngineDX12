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

class Application;

class Engine {
public:
	virtual ~Engine();

	bool Initialize(const RenderWindowConfig& cfg);

	ApplicationOptions& GetConfig();

	void ShowWindow();
	bool ProcessMessages();

	std::shared_ptr<BaseEngineLogic> GetGameLogic();
	std::shared_ptr<IRenderer> GetRenderer();
	static std::shared_ptr<Engine> GetEngine();
	//std::shared_ptr<HumanView> GetHumanView();
	//std::shared_ptr<HumanView> GetHumanViewByName(std::string name);

	void Update(IEventDataPtr pEventData);
	void RenderFrame();
	//virtual bool VLoadGame();
	//int Modal(std::shared_ptr<IScreenElement> pModalScreen, int defaultAnswer);

protected:
	Engine();
	//virtual std::unique_ptr<BaseEngineLogic> VCreateGameAndView();
	virtual void VRegisterEvents();
	virtual void RegisterAllDelegates();

private:

	ApplicationOptions m_options;

	std::shared_ptr<IRenderer> m_renderer;
	std::shared_ptr<BaseEngineLogic> m_game;

	static std::shared_ptr<Engine> m_pEngine;
};