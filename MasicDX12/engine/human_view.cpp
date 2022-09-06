#include "human_view.h"

#include "../application.h"
#include "engine.h"
#include "../actors/camera_component.h"

#include <DirectXCollision.h>

using namespace std::literals;
const std::string HumanView::g_Name = "Level"s;

HumanView::HumanView() {
	m_process_manager = std::make_unique<ProcessManager>();

	m_pointer_radius = 1.0f;
	m_view_id = 0xffffffff;

	RegisterAllDelegates();
	m_base_game_state = BaseEngineState::BGS_Initializing;

	
	m_scene.reset(new ScreenElementScene());
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

void HumanView::VOnRender(const GameTimerDelta& delta) {
	m_current_tick = delta.GetTotalDuration();
	if (m_current_tick == m_last_draw) { return; }

	if (!m_can_draw) { return; }

	const auto one_frame_time = 0.016ms;
	if (m_run_full_speed || ((m_current_tick - m_last_draw) > one_frame_time)) {
		auto renderer = Engine::GetEngine()->GetRenderer();
		if (renderer->VPreRender()) {
			m_screen_elements.sort(SortBy_SharedPtr_Content<IScreenElement>());
			for (ScreenElementList::iterator i = m_screen_elements.begin(); i != m_screen_elements.end(); ++i) {
				if ((*i)->VIsVisible()) {
					(*i)->VOnRender(delta);
				}
			}
			VRenderText();

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
}

EngineViewType HumanView::VGetType() {
	return EngineViewType::GameView_Human;
}

EngineViewId HumanView::VGetId() const {
	return m_view_id;
}

void HumanView::VOnAttach(EngineViewId vid, ActorId aid) {
	m_view_id = vid;
	m_actor_id = aid;
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

void HumanView::VSetControlledActor(ActorId actorId) {
	m_actor_id = actorId;
}

std::shared_ptr<CameraNode> HumanView::VGetCamera() {
	if (!m_camera.expired()) {
		return m_camera.lock();
	}
	return std::shared_ptr<CameraNode>();
}

void HumanView::VSetCameraByName(std::string camera_name) {
	WeakActorPtr weak_camera_actor = Engine::GetEngine()->GetGameLogic()->VGetActorByName(camera_name);
	if (auto camera_actor = weak_camera_actor.lock()) {
		auto weak_camera_component = camera_actor->GetComponent<CameraComponent>();
		if (auto camera_component = weak_camera_component.lock()) {
			m_camera = camera_component->VGetSceneNode();
		}
	}
}

const std::string& HumanView::VGetName() {
	return g_Name;
}

void HumanView::GameStateDelegate(IEventDataPtr pEventData) {}

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
		}
	}
	VPushElement(m_scene);
	return false;
}

void HumanView::VRenderText() {}

void HumanView::RegisterAllDelegates() {}

void HumanView::RemoveAllDelegates() {}
