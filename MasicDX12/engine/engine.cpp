#include "engine.h"

#include "../events/evt_data_os_message.h"
#include "../events/evt_data_update_tick.h"
#include "../graphics/d3d12_renderer.h"
#include "../graphics/directx12_wrappers/command_queue.h"
#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/directx12_wrappers/swap_chain.h"
#include "../graphics/directx12_wrappers/texture.h"

#include "../application.h"

std::shared_ptr<Engine> Engine::m_pEngine = nullptr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Engine::Engine() {}

Engine::~Engine() {}

void Engine::Destroy() {
	if (m_pEngine) {
		m_pEngine->m_game.reset();
		m_pEngine.reset();
	}
}

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
	
	m_renderer = std::make_shared<D3DRenderer12>();
	if (!m_renderer->Initialize(render_window)) return false;
	
	m_renderer->VOnRestore();

	m_game = VCreateGameAndView();
	if (!m_game) return false;

	return true;
}

ApplicationOptions& Engine::GetConfig() {
	return m_options;
}

std::shared_ptr<BaseEngineLogic> Engine::VCreateGameAndView() {
	std::shared_ptr pGame = std::make_shared<BaseEngineLogic>();
	pGame->Init();
	
	return pGame;
}

void Engine::ShowWindow() {
	return m_renderer->GetRenderWindow()->Show();
}

bool Engine::ProcessMessages() {
	return m_renderer->GetRenderWindow()->ProcessMessages();
}

void Engine::Update(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Update_Tick> pCastEventData = std::static_pointer_cast<EvtData_Update_Tick>(pEventData);
	if (m_game) {
		IEventManager::Get()->VUpdate();
		GameTimer& timer = Application::Get().GetTimer();
		m_game->VOnUpdate(timer);
	}
}

void Engine::RenderFrame() {
	if (!m_game) return;

	std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(GetRenderer());
	std::shared_ptr<Device> device = renderer->GetDevice();
	CommandQueue& command_queue = device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	std::shared_ptr<CommandList> command_list = command_queue.GetCommandList();

	for (GameViewList::iterator i = m_game->m_game_views.begin(), end = m_game->m_game_views.end(); i != end; ++i) {
		(*i)->VOnRender(Application::Get().GetTimer(), command_list);
	}
	CommandQueue::FenceValueType fence_value = command_queue.ExecuteCommandList(command_list);
	m_renderer->VPresent();
}

LRESULT Engine::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (uMsg) {
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MOUSEWHEEL:
		case MM_JOY1BUTTONDOWN:
		case MM_JOY1BUTTONUP:
		case MM_JOY1MOVE:
		case MM_JOY1ZMOVE:
		case MM_JOY2BUTTONDOWN:
		case MM_JOY2BUTTONUP:
		case MM_JOY2MOVE:
		case MM_JOY2ZMOVE: {
			if (!m_game) break;
			for (GameViewList::reverse_iterator i = m_game->m_game_views.rbegin(); i != m_game->m_game_views.rend(); ++i) {
				if ((*i)->VOnMsgProc(hwnd, uMsg, wParam, lParam)) {
					result = true;
					break;
				}
			}
		}
		break;
	}
	return result;
}

void Engine::VRegisterEvents() {}

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