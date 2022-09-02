#include "engine.h"

#include "../events/evt_data_os_message.h"
#include "../events/evt_data_update_tick.h"
#include "../graphics/d3d12_renderer.h"
#include "../application.h"

std::shared_ptr<Engine> Engine::m_pEngine = nullptr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Engine::Engine() {}

Engine::~Engine() {}

bool Engine::Initialize(const RenderWindowConfig& cfg) {
	if (!DirectX::XMVerifyCPUSupport()) {
		MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
#if defined( _DEBUG )
	Device::EnableDebugLayer();
#endif

	m_options = cfg.options;

	VRegisterEvents();
	RegisterAllDelegates();

	std::shared_ptr<WindowSurface> render_window = Application::Get().CreateRenderWindow(cfg);
	if (!render_window) return false;
	
	m_renderer = std::make_unique<D3DRenderer12>();
	if (!m_renderer->Initialize(render_window)) return false;
	
	m_renderer->VOnRestore();

	//m_game = VCreateGameAndView();
	//if (!m_game) {
	//	return false;
	//}

	return true;
}

ApplicationOptions& Engine::GetConfig() {
	return m_options;
}

//std::unique_ptr<BaseEngineLogic> Engine::VCreateGameAndView() {
//	std::unique_ptr pGame = std::make_unique<XLogic>();
//	pGame->Init();
//
//	std::shared_ptr<IEngineView> menuView(new MainMenuView(m_renderer.get()));
//	pGame->VAddView(menuView);
//
//	return pGame;
//}

bool Engine::ProcessMessages() {
	return m_renderer->GetRenderWindow()->ProcessMessages();
}

void Engine::Update(IEventDataPtr pEventData) {
	
}

void Engine::RenderFrame() {
	//BaseEngineLogic* pGame = g_pApp->m_game.get();
	//for (GameViewList::iterator i = pGame->m_game_views.begin(), end = pGame->m_game_views.end(); i != end; ++i) {
	//	(*i)->VOnRender(m_timer.TotalTime(), m_timer.DeltaTime());
	//}
	//m_renderer->VPresent();
}

void Engine::VRegisterEvents() {
	//REGISTER_EVENT(EvtData_OS_Message);
	//REGISTER_EVENT(EvtData_Environment_Loaded);
	//REGISTER_EVENT(EvtData_New_Actor);
	//REGISTER_EVENT(EvtData_Move_Actor);
	//REGISTER_EVENT(EvtData_Destroy_Actor);
	//REGISTER_EVENT(EvtData_Request_New_Actor);
}

void Engine::RegisterAllDelegates() {
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VAddListener({ connect_arg<&Engine::Update>, this }, EvtData_Update_Tick::sk_EventType);
	//IEventManager* pGlobalEventManager = IEventManager::Get();
	//pGlobalEventManager->VAddListener({ connect_arg<&Engine::OSMessageDelegate>, this }, EvtData_OS_Message::sk_EventType);
}

//int Engine::Modal(std::shared_ptr<IScreenElement> pModalScreen, int defaultAnswer) {
//	auto pView = GetHumanView();
//	if (!pView) { return defaultAnswer; }
//	pView->VPushElement(pModalScreen);
//	m_has_modal_dialog += 1;
//	return 0;
//}

std::shared_ptr<BaseEngineLogic> Engine::GetGameLogic() {
	return m_game;
}

std::shared_ptr<IRenderer> Engine::GetRenderer() {
	return m_renderer;
}

//bool Engine::VLoadGame() {
//	return m_game->VLoadGame("MainMenu.xml");
//}

//std::shared_ptr<HumanView> Engine::GetHumanView() {
//	std::shared_ptr<HumanView> pView;
//	if (!m_game) { return pView; }
//	for (GameViewList::iterator i = m_game->m_game_views.begin(); i != m_game->m_game_views.end(); ++i) {
//		if ((*i)->VGetType() == EngineViewType::GameView_Human) {
//			pView = std::dynamic_pointer_cast<HumanView>(*i);
//			break;
//		}
//	}
//	return pView;
//}

//std::shared_ptr<HumanView> Engine::GetHumanViewByName(std::string name) {
//	std::shared_ptr<HumanView> pView;
//	if (!m_game) { return pView; }
//	for (GameViewList::iterator i = m_game->m_game_views.begin(); i != m_game->m_game_views.end(); ++i) {
//		if ((*i)->VGetType() == EngineViewType::GameView_Human && (*i)->VGetName() == name) {
//			pView = std::dynamic_pointer_cast<HumanView>(*i);
//			break;
//		}
//	}
//	return pView;
//}

std::shared_ptr<Engine> Engine::GetEngine() {
	if (m_pEngine == nullptr) {
		m_pEngine.reset(new Engine());
	}
	return m_pEngine;
}