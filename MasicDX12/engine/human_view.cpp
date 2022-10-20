#include "human_view.h"

#include "../application.h"
#include "engine.h"
#include "../actors/camera_component.h"
#include "../events/evt_data_update_tick.h"
#include "../events/evt_data_new_scene_component.h"
#include "../events/evt_data_destroy_scene_component.h"
#include "../graphics/d3d12_renderer.h"
#include "../graphics/i_renderer.h"
#include "../graphics/directx12_wrappers/command_queue.h"
#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/directx12_wrappers/swap_chain.h"
#include "../graphics/directx12_wrappers/texture.h"

#include <DirectXCollision.h>

using namespace std::literals;
const std::string HumanView::g_Name = "Level"s;

HumanView::HumanView() {
	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());
	std::shared_ptr<Device> device = renderer->GetDevice();

	m_process_manager = std::make_shared<ProcessManager>();

	m_pointer_radius = 1.0f;
	m_view_id = 0xffffffff;

	m_bShow_ui = false;
	m_bShow_debug_ui = Application::Get().GetApplicationOptions().DebugUI;
	if (m_bShow_debug_ui) {
		m_actor_menu_ui = std::make_shared<ActorMenuUI>(m_process_manager);
		VPushElement(m_actor_menu_ui);
	}

	RegisterAllDelegates();
	m_base_game_state = BaseEngineState::BGS_Initializing;
	
	m_scene.reset(new ScreenElementScene());

	m_current_tick = {};
	m_last_draw = {};

	m_hwnd = renderer->GetRenderWindow()->GetHWND();
	m_gui = device->CreateGUI(m_hwnd, renderer->GetRenderTarget());
}

HumanView::~HumanView() {
	RemoveAllDelegates();
}

bool HumanView::LoadGame(const pugi::xml_node& pLevelData) {
	return VLoadGameDelegate(pLevelData);
}

HRESULT HumanView::VOnRestore() {
	HRESULT hr = S_OK;
	for (ScreenElementList::iterator i = m_screen_elements.begin(); i != m_screen_elements.end(); ++i) {
		hr = (*i)->VOnRestore();
		if (FAILED(hr)) { return hr; }
	}

	return hr;
}

HRESULT HumanView::VOnLostDevice() {
	HRESULT hr;
	for (ScreenElementList::iterator i = m_screen_elements.begin(); i != m_screen_elements.end(); ++i) {
		hr = (*i)->VOnLostDevice();
		if (FAILED(hr)) { return hr; }
	}

	return S_OK;
}

void HumanView::VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList> command_list) {
	m_current_tick = delta.GetTotalDuration();
	if (m_current_tick == m_last_draw) { return; }
	if (!m_can_draw) { return; }

	const auto one_frame_time = 0.016ms;
	if (m_run_full_speed || ((m_current_tick - m_last_draw) > one_frame_time)) {
		auto renderer = Engine::GetEngine()->GetRenderer();
		m_gui->NewFrame();
		if (renderer->VPreRender(command_list)) {
			m_screen_elements.sort(SortBy_SharedPtr_Content<IScreenElement>());
			for (ScreenElementList::iterator i = m_screen_elements.begin(); i != m_screen_elements.end(); ++i) {
				if ((*i)->VIsVisible()) {
					(*i)->VOnRender(delta, command_list);
				}
			}
			VRenderText();
			m_gui->Render(command_list, renderer->GetRenderTarget());
			m_last_draw = m_current_tick;
			renderer->VPostRender();
		}
	}
}

void HumanView::VOnUpdate(const GameTimerDelta& delta) {
	m_process_manager->UpdateProcesses(delta);
	for (ScreenElementList::iterator i = m_screen_elements.begin(); i != m_screen_elements.end(); ++i) {
		(*i)->VOnUpdate(delta);
	}
	if (m_pFree_camera_controller) {
		m_pFree_camera_controller->OnUpdate(delta);
	}
}

EngineViewType HumanView::VGetType() {
	return EngineViewType::GameView_Human;
}

EngineViewId HumanView::VGetId() const {
	return m_view_id;
}

void HumanView::VOnAttach(EngineViewId vid, ActorId aid) {
	m_view_id = vid;
	if (aid != INVALID_ACTOR_ID) {
		m_actor = Engine::GetEngine()->GetGameLogic()->VGetActor(aid);
	}
}

LRESULT HumanView::VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) {
	for (ScreenElementList::reverse_iterator i = m_screen_elements.rbegin(); i != m_screen_elements.rend(); ++i) {
		if ((*i)->VIsVisible()) {
			if ((*i)->VOnMsgProc(m_hWnd, m_uMsg, m_wParam, m_lParam)) {
				return 1;
			}
		}
	}

	LRESULT result = 0;
	switch (m_uMsg) {
		case WM_KEYDOWN: {
			if (((unsigned int)m_wParam) == (VK_ESCAPE)) {
				Application::Get().Quit();
			}
			if (m_keyboard_handlers.size()) {
				for (auto& handler : m_keyboard_handlers) {
					result |= handler->VOnKeyDown(static_cast<const BYTE>(m_wParam));
				}
			}
		}
		break;
		case WM_KEYUP: {
			if (m_keyboard_handlers.size()) {
				for (auto& handler : m_keyboard_handlers) {
					result |= handler->VOnKeyUp(static_cast<const BYTE>(m_wParam));
				}
			}
			result = true;
		}
		break;
		case WM_MOUSEMOVE: {
			if (m_pointer_handlers.size()) {
				for (auto& handler : m_pointer_handlers) {
					result |= handler->VOnPointerMove(LOWORD(m_lParam), HIWORD(m_lParam), 1);
				}
			}
		}
		break;
		case WM_LBUTTONDOWN: {
			if (m_pointer_handlers.size()) {
				SetCapture(m_hWnd);
				for (auto& handler : m_pointer_handlers) {
					result |= handler->VOnPointerButtonDown(LOWORD(m_lParam), HIWORD(m_lParam), 1, "PointerLeft");
				}
			}
		}
		break;
		case WM_LBUTTONUP: {
			if (m_pointer_handlers.size()) {
				ReleaseCapture();
				for (auto& handler : m_pointer_handlers) {
					result |= handler->VOnPointerButtonUp(LOWORD(m_lParam), HIWORD(m_lParam), 1, "PointerLeft");
				}
			}
		}
		break;
		case WM_RBUTTONDOWN: {
			if (m_pointer_handlers.size()) {
				SetCapture(m_hWnd);
				for (auto& handler : m_pointer_handlers) {
					result |= handler->VOnPointerButtonDown(LOWORD(m_lParam), HIWORD(m_lParam), 1, "PointerRight");
				}
			}
		}
		break;
		case WM_RBUTTONUP: {
			if (m_pointer_handlers.size()) {
				ReleaseCapture();
				for (auto& handler : m_pointer_handlers) {
					result = handler->VOnPointerButtonUp(LOWORD(m_lParam), HIWORD(m_lParam), 1, "PointerRight");
				}
			}
		}
		break;
		break;
		case WM_CHAR: {}
		break;
	}

	return 0;
}

void HumanView::VPushElement(std::shared_ptr<IScreenElement> pElement) {
	m_screen_elements.push_front(pElement);
}

void HumanView::VRemoveElement(std::shared_ptr<IScreenElement> pElement) {
	m_screen_elements.remove(pElement);
}

void HumanView::VActivateScene(bool is_active) {
	if (m_scene) {
		m_scene->ActivateScene(is_active);
	}
}

void HumanView::VCanDraw(bool is_can_draw) {
	m_can_draw = is_can_draw;
}

void HumanView::TogglePause(bool active) {}

void HumanView::HandleGameState(BaseEngineState newState) {}

void HumanView::VSetControlledActor(std::shared_ptr<Actor> actor) {
	m_pTeapot = actor;
	if (m_pTeapot.expired()) {
		m_keyboard_handlers.clear();
		m_pointer_handlers.clear();
		//m_pFreeCameraController.reset(new MovementController(m_camera, 0, 0, false, true));
		//m_keyboard_handlers.push_back(m_pFreeCameraController);
		//m_pointer_handlers.push_back(m_pFreeCameraController);
		//if (auto camera = m_camera.lock()) {
		//	camera->SetTarget(nullptr);
		//}
		return;
	}
	else {
		m_keyboard_handlers.clear();
		m_pointer_handlers.clear();
		m_pFree_camera_controller.reset();
		m_keyboard_handlers.push_back(m_pFree_camera_controller);
		m_pointer_handlers.push_back(m_pFree_camera_controller);

		//m_camera->SetTarget(m_pTeapot);
	}
	m_actor = actor;
}

std::shared_ptr<BasicCameraNode> HumanView::VGetCamera() {
	if (!m_camera.expired()) {
		return m_camera.lock();
	}
	return std::shared_ptr<BasicCameraNode>();
}

std::shared_ptr<Scene> HumanView::VGetScene() {
	return std::static_pointer_cast<Scene>(m_scene);
}

void HumanView::VSetCameraByName(std::string camera_name) {
	WeakActorPtr weak_camera_actor = Engine::GetEngine()->GetGameLogic()->VGetActorByName(camera_name);
	if (auto camera_actor = weak_camera_actor.lock()) {
		auto weak_camera_component = camera_actor->GetComponent<CameraComponent>();
		if (auto camera_component = weak_camera_component.lock()) {
			m_camera = camera_component->VGetCameraNode();
		}
	}
}

const std::string& HumanView::VGetName() {
	return g_Name;
}

void HumanView::GameStateDelegate(IEventDataPtr pEventData) {}

void HumanView::NewSceneNodeComponentDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_New_Scene_Component> pCastEventData = std::static_pointer_cast<EvtData_New_Scene_Component>(pEventData);
	m_scene->AddChild(pCastEventData->GetSceneNode().lock());
}

void HumanView::DestroySceneNodeComponentDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Destroy_Scene_Component> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Scene_Component>(pEventData);
	m_scene->RemoveChild(pCastEventData->GetSceneNode().lock());
}

bool HumanView::VLoadGameDelegate(const pugi::xml_node& pLevel_data) {
	using namespace std::literals;
	pugi::xml_node scene_config_node = pLevel_data.child("Scene");
	if (scene_config_node) {
		for (pugi::xml_node node = scene_config_node.first_child(); node; node = node.next_sibling()) {
			std::string param_name = node.name();

			if (param_name == "Camera"s) {
				std::string camera_name = node.child("SelectName").text().as_string();
				VSetCameraByName(camera_name);
			}

			if (param_name == "BackgroundColor"s) {
				std::shared_ptr<Engine> engine = Engine::GetEngine();
				std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());

				DirectX::XMFLOAT3 default_color = { 1.0f, 1.0f, 1.0f };
				DirectX::XMFLOAT3 bg_color = colorfromattr3f(node, default_color);
				renderer->VSetClearColor4f(bg_color.x, bg_color.y, bg_color.z, 1.0f);
			}

			if (param_name == "Fog"s) {

				DirectX::XMFLOAT3 default_fog_color = { 1.0f, 1.0f, 1.0f };
				DirectX::XMFLOAT3 fog_color = colorfromattr3f(node.child("FogColor"), default_fog_color);
				float fog_range = node.child("FogRange").text().as_float();
				float fog_start = node.child("FogStart").text().as_float();
				Scene::SceneConfig sc_cfg = {};
				sc_cfg.FogColor = DirectX::XMFLOAT4(fog_color.x, fog_color.y, fog_color.z, 1.0f);
				sc_cfg.FogStart = fog_start;
				sc_cfg.FogRange = fog_range;
				m_scene->SetSceneConfig(sc_cfg);
			}
		}
	}
	VPushElement(m_scene);
	m_keyboard_handlers.clear();
	m_pointer_handlers.clear();
	//m_pFreeCameraController.reset(new MovementController(m_camera, 0, 0, false, true));
	//m_keyboard_handlers.push_back(m_pFreeCameraController);
	//m_pointer_handlers.push_back(m_pFreeCameraController);

	m_scene->VOnRestore();
	return true;
}

void HumanView::VRenderText() {}

void HumanView::RegisterAllDelegates() {
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VAddListener({ connect_arg<&HumanView::NewSceneNodeComponentDelegate>, this }, EvtData_New_Scene_Component::sk_EventType);
	pGlobalEventManager->VAddListener({ connect_arg<&HumanView::DestroySceneNodeComponentDelegate>, this }, EvtData_Destroy_Scene_Component::sk_EventType);
}

void HumanView::RemoveAllDelegates() {
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VRemoveListener({ connect_arg<&HumanView::NewSceneNodeComponentDelegate>, this }, EvtData_New_Scene_Component::sk_EventType);
	pGlobalEventManager->VRemoveListener({ connect_arg<&HumanView::DestroySceneNodeComponentDelegate>, this }, EvtData_Destroy_Scene_Component::sk_EventType);
}
