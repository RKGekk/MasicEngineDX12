#include "base_engine_logic.h"

#include "../events/evt_data_request_destroy_actor.h"
#include "../events/evt_data_request_new_actor.h"
#include "../events/evt_data_move_actor.h"
#include "../events/evt_data_new_actor.h"
#include "../events/evt_data_request_start_game.h"
#include "../events/evt_data_environment_loaded.h"
#include "../events/evt_data_sphere_particle_contact.h"
#include "../events/evt_data_destroy_actor.h"
#include "../events/evt_data_new_scene_component.h"
#include "../actors/actor_factory.h"
#include "level_manager.h"
#include "engine.h"
#include "../application.h"
#include "../actors/transform_component.h"
#include "../processes/exec_process.h"
#include "../processes/delay_process.h"

BaseEngineLogic::BaseEngineLogic() {
	m_last_actor_id = 0;
	m_process_manager = std::make_unique<ProcessManager>();
	m_random.Randomize();
	m_state = BaseEngineState::BGS_Initializing;
	m_render_diagnostics = false;
	m_expected_players = 0;
	m_human_players_attached = 0;
	m_human_games_loaded = 0;
	m_actor_factory = nullptr;
	m_life_time = {};
	m_level_manager = std::make_unique<LevelManager>();
	m_level_manager->Initialize();
	//m_physics = std::make_unique<XPhysics>();
	//m_physics->VInitialize();
}

BaseEngineLogic::~BaseEngineLogic() {
	while (!m_game_views.empty()) {
		m_game_views.pop_front();
	}

	for (auto it = m_actors.begin(); it != m_actors.end(); ++it) {
		it->second->Destroy();
	}
	m_actors.clear();
	m_actors_names.clear();
	m_components.clear();

	RemoveAllDelegates();
}

bool BaseEngineLogic::Init() {
	m_actor_factory = VCreateActorFactory();
	VRegisterEvents();
	RegisterAllDelegates();

	return true;
}

ActorId BaseEngineLogic::GetNewActorID() {
	return ++m_last_actor_id;
}

MTRandom& BaseEngineLogic::GetRNG() {
	return m_random;
}

void BaseEngineLogic::VAddView(std::shared_ptr<IEngineView> pView, ActorId actorId) {
	int viewId = static_cast<int>(m_game_views.size());
	m_game_views.push_back(pView);
	pView->VOnAttach(viewId, actorId);
	pView->VOnRestore();
}

void BaseEngineLogic::VRemoveView(std::shared_ptr<IEngineView> pView) {
	m_game_views.remove(pView);
}

StrongActorPtr BaseEngineLogic::VCreateActor(const std::string& actor_resource, const pugi::xml_node& overrides, DirectX::FXMMATRIX initial_transform, const ActorId servers_actorId) {
	DirectX::XMFLOAT4X4 t;
	DirectX::XMStoreFloat4x4(&t, initial_transform);
	return VCreateActor(actor_resource, overrides, &t, servers_actorId);
}

StrongActorPtr BaseEngineLogic::VCreateActor(const std::string& actor_resource, const pugi::xml_node& overrides, const DirectX::XMFLOAT4X4* initial_transform, const ActorId servers_actorId) {
	StrongActorPtr pActor = m_actor_factory->CreateActor(actor_resource, overrides, initial_transform, servers_actorId);
	if (pActor) {
		ActorId actid = pActor->GetId();
		m_actors.insert(std::make_pair(actid, pActor));
		if (pActor->GetName() != "NoName") { m_actors_names.insert(std::make_pair(pActor->GetName(), pActor)); }
		const ActorComponents& components = pActor->GetComponents();
		for (const auto& [k, v] : components) {
			m_components[k].insert(actid);
		}
		return pActor;
	}
	else {
		return StrongActorPtr();
	}
}

void BaseEngineLogic::VDestroyActor(const ActorId actorId) {
	auto findIt = m_actors.find(actorId);
	if (findIt != m_actors.end()) {
		if (findIt->second->GetName() != "NoName") { m_actors_names.erase(findIt->second->GetName()); }
		const ActorComponents& components = findIt->second->GetComponents();
		for (const auto& [k, v] : components) {
			m_components[k].erase(actorId);
		}
		findIt->second->Destroy();
		m_actors.erase(findIt);
	}
}

WeakActorPtr BaseEngineLogic::VGetActor(const ActorId actorId) {
	ActorMap::iterator findIt = m_actors.find(actorId);
	if (findIt != m_actors.end()) {
		return findIt->second;
	}
	return WeakActorPtr();
}

WeakActorPtr BaseEngineLogic::VGetActorByName(const std::string& actor_name) {
	auto findIt = m_actors_names.find(actor_name);
	if (findIt != m_actors_names.end()) {
		return findIt->second;
	}
	return WeakActorPtr();
}

const std::unordered_set<ActorId>& BaseEngineLogic::VGetActorsByComponent(ComponentId cid) {
	return m_components.at(cid);
}

bool BaseEngineLogic::VCheckActorsExistByComponent(ComponentId cid) {
	return m_components.count(cid);
}

void BaseEngineLogic::VModifyActor(const ActorId actorId, const pugi::xml_node& overrides) {
	if (!m_actor_factory) { return; }

	auto findIt = m_actors.find(actorId);
	if (findIt != m_actors.end()) {
		m_actor_factory->ModifyActor(findIt->second, overrides);
	}
}

void BaseEngineLogic::VMoveActor(const ActorId id, DirectX::FXMMATRIX mat) {
	StrongActorPtr pActor = MakeStrongPtr(VGetActor(id));
	if (pActor) {
		std::shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
	}
}

void BaseEngineLogic::VMoveActor4x4f(const ActorId id, const DirectX::XMFLOAT4X4& mat) {
	VMoveActor(id, DirectX::XMLoadFloat4x4(&mat));
}

std::string BaseEngineLogic::GetActorXml(const ActorId id) {
	StrongActorPtr pActor = MakeStrongPtr(VGetActor(id));
	if (pActor) {
		return pActor->ToXML();
	}

	return std::string();
}

std::shared_ptr<CameraNode> BaseEngineLogic::GetActiveCamera() {
	return m_active_camera;
}

const LevelManager& BaseEngineLogic::GetLevelManager() {
	return *m_level_manager;
}

std::shared_ptr<IEnginePhysics> BaseEngineLogic::VGetGamePhysics() {
	return m_physics;
}

bool BaseEngineLogic::VLoadGame(const std::string& level_resource) {
	pugi::xml_document xml_doc;
	pugi::xml_parse_result parse_res = xml_doc.load_file(level_resource.c_str());
	if (!parse_res) return false;

	pugi::xml_node root_node = xml_doc.root();
	if (!root_node) return false;

	pugi::xml_node world_node = root_node.child("World");
	if (!world_node) return false;

	pugi::xml_node actors_node = world_node.child("Actors");
	if (actors_node) {
		for (pugi::xml_node node = actors_node.first_child(); node; node = node.next_sibling()) {
			const char* actor_resource = node.attribute("resource").value();
			StrongActorPtr pActor = VCreateActor(actor_resource, node, nullptr);
		}
	}

	for (auto it = m_game_views.begin(); it != m_game_views.end(); ++it) {
		std::shared_ptr<IEngineView> pView = *it;
		if (pView->VGetType() == EngineViewType::GameView_Human) {
			std::shared_ptr<HumanView> pHumanView = std::static_pointer_cast<HumanView, IEngineView>(pView);
			pHumanView->LoadGame(world_node);
		}
	}

	if (!VLoadGameDelegate(world_node)) { return false; }

	//std::shared_ptr<EvtData_Environment_Loaded> pNewGameEvent(new EvtData_Environment_Loaded);
	//IEventManager::Get()->VTriggerEvent(pNewGameEvent);

	return true;
}

bool BaseEngineLogic::VLoadGame(const std::string& level_resource, std::shared_ptr<HumanView> pHumanView) {
	pugi::xml_document xml_doc;
	pugi::xml_parse_result parse_res = xml_doc.load_file(level_resource.c_str());
	if (!parse_res) return false;

	pugi::xml_node root_node = xml_doc.root();
	if (!root_node) return false;

	pugi::xml_node actors_node = root_node.child("Actors");
	if (actors_node) {
		for (pugi::xml_node node = actors_node.first_child(); node; node = node.next_sibling()) {
			const char* actor_resource = node.attribute("resource").value();
			StrongActorPtr pActor = VCreateActor(actor_resource, node, nullptr);
		}
	}

	pHumanView->LoadGame(root_node);
	if (!VLoadGameDelegate(root_node)) return false;

	return true;
}

void BaseEngineLogic::VOnUpdate(const GameTimerDelta& delta) {
	using namespace std::literals;
	GameClockDuration delta_duration = delta.GetDeltaDuration();
	GameClockDuration total_duration = delta.GetTotalDuration();
	m_life_time += delta_duration;
	m_process_manager->UpdateProcesses(delta);

	switch (m_state) {
		case BaseEngineState::BGS_Initializing: {
			std::shared_ptr<IEngineView> menuView = std::make_shared<HumanView>();
			VAddView(menuView);
			VChangeState(BaseEngineState::BGS_MainMenu);
		}
		break;
		case BaseEngineState::BGS_MainMenu: {}
		break;
		case BaseEngineState::BGS_LoadingGameEnvironment: {}
		break;
		case BaseEngineState::BGS_Running: {
			const static GameTimerDelta const_update_time = GameTimerDelta(std::chrono::duration_cast<GameClockDuration>(16.0ms), std::chrono::duration_cast<GameClockDuration>(16.0ms));
			if (delta_duration > 16.0ms) { m_physics->VOnUpdate(const_update_time); }
			else { m_physics->VOnUpdate(delta); }
			m_physics->VSyncVisibleScene();
		}
		break;
	}

	for (ActorMap::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
		it->second->Update(delta);
	}

	for (GameViewList::iterator it = m_game_views.begin(); it != m_game_views.end(); ++it) {
		(*it)->VOnUpdate(delta);
	}
}

void BaseEngineLogic::VChangeState(BaseEngineState newState) {
	using namespace std::literals;
	switch (newState) {
		case BaseEngineState::BGS_MainMenu: {
			if (!VLoadGame("main_menu.xml"s)) {
				Application::Get().Quit();
			}
		}
		break;
		case BaseEngineState::BGS_LoadingGameEnvironment: {
			std::shared_ptr<ExecProcess> execOne = std::make_shared<ExecProcess>([]() {
				auto main_menu = Engine::GetEngine()->GetGameLogic()->GetHumanView();
				main_menu->VActivateScene(false);
				return true;
			});
			std::shared_ptr<DelayProcess> delay = std::make_shared<DelayProcess>(std::chrono::duration_cast<GameClockDuration>(2.0s), [](const GameTimerDelta& delta, float n) {
				return true;
			});
			std::shared_ptr<ExecProcess> exec2 = std::make_shared<ExecProcess>([]() {
				std::shared_ptr<HumanView> gameView(new HumanView());
				Engine::GetEngine()->GetGameLogic()->VLoadGame("World.xml", gameView);
				gameView->VCanDraw(false);
				Engine::GetEngine()->GetGameLogic()->VAddView(gameView);
				return true;
			});
			std::shared_ptr<ExecProcess> exec3 = std::make_shared<ExecProcess>([]() {
				Application::Get().GetTimer().Reset();
				std::shared_ptr<EvtData_Environment_Loaded> pEvent(new EvtData_Environment_Loaded());
				IEventManager::Get()->VTriggerEvent(pEvent);
				return true;
			});
			execOne->AttachChild(delay);
			delay->AttachChild(exec2);
			exec2->AttachChild(exec3);
			m_process_manager->AttachProcess(execOne);
		}
		break;
		case BaseEngineState::BGS_Running: {}
		break;
	}

	m_state = newState;
}

const BaseEngineState BaseEngineLogic::GetState() const {
	return m_state;
}

std::shared_ptr<HumanView> BaseEngineLogic::GetHumanView() {
	std::shared_ptr<HumanView> pView;
	for (GameViewList::iterator i = m_game_views.begin(); i != m_game_views.end(); ++i) {
		if ((*i)->VGetType() == EngineViewType::GameView_Human) {
			pView = std::dynamic_pointer_cast<HumanView>(*i);
			break;
		}
	}
	return pView;
}

std::shared_ptr<HumanView> BaseEngineLogic::GetHumanViewByName(std::string name) {
	std::shared_ptr<HumanView> pView;
	for (GameViewList::iterator i = m_game_views.begin(); i != m_game_views.end(); ++i) {
		if ((*i)->VGetType() == EngineViewType::GameView_Human && (*i)->VGetName() == name) {
			pView = std::dynamic_pointer_cast<HumanView>(*i);
			break;
		}
	}
	return pView;
}

void BaseEngineLogic::AttachProcess(StrongProcessPtr pProcess) {
	if (m_process_manager) {
		m_process_manager->AttachProcess(pProcess);
	}
}

void BaseEngineLogic::RequestDestroyActorDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Request_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Request_Destroy_Actor>(pEventData);
	VDestroyActor(pCastEventData->GetActorId());
}

void BaseEngineLogic::MoveActorDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Move_Actor> pCastEventData = std::static_pointer_cast<EvtData_Move_Actor>(pEventData);
	VMoveActor(pCastEventData->GetId(), pCastEventData->GetMatrix());
}

void BaseEngineLogic::RequestNewActorDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Request_New_Actor> pCastEventData = std::static_pointer_cast<EvtData_Request_New_Actor>(pEventData);

	StrongActorPtr pActor = VCreateActor(pCastEventData->GetActorResource(), pugi::xml_node(), pCastEventData->GetInitialTransform(), pCastEventData->GetServerActorId());
}

void BaseEngineLogic::RequestStartGameDelegate(IEventDataPtr pEventData) {}

void BaseEngineLogic::EnvironmentLoadedDelegate(IEventDataPtr pEventData) {}

void BaseEngineLogic::SphereParticleContactDelegate(IEventDataPtr pEventData) {}

std::unique_ptr<ActorFactory> BaseEngineLogic::VCreateActorFactory() {
	return std::make_unique<ActorFactory>();
}

bool BaseEngineLogic::VLoadGameDelegate(const pugi::xml_node& pLevelData) {
	return true;
}

void BaseEngineLogic::RegisterAllDelegates() {
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VAddListener({ connect_arg<&BaseEngineLogic::RequestDestroyActorDelegate>, this }, EvtData_Request_Destroy_Actor::sk_EventType);
	pGlobalEventManager->VAddListener({ connect_arg<&BaseEngineLogic::RequestNewActorDelegate>, this }, EvtData_Request_New_Actor::sk_EventType);
	pGlobalEventManager->VAddListener({ connect_arg<&BaseEngineLogic::MoveActorDelegate>, this }, EvtData_Move_Actor::sk_EventType);
	pGlobalEventManager->VAddListener({ connect_arg<&BaseEngineLogic::RequestStartGameDelegate>, this }, EvtData_Request_Start_Game::sk_EventType);
	pGlobalEventManager->VAddListener({ connect_arg<&BaseEngineLogic::EnvironmentLoadedDelegate>, this }, EvtData_Environment_Loaded::sk_EventType);
	pGlobalEventManager->VAddListener({ connect_arg<&BaseEngineLogic::SphereParticleContactDelegate>, this }, EvtData_Sphere_Particle_Contact::sk_EventType);
}

void BaseEngineLogic::VRegisterEvents() {
	//REGISTER_EVENT(EvtData_Environment_Loaded);
	//REGISTER_EVENT(EvtData_Move_Actor);
	//REGISTER_EVENT(EvtData_Destroy_Actor);
	REGISTER_EVENT(EvtData_Request_New_Actor);
}

void BaseEngineLogic::RemoveAllDelegates() {
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VRemoveListener({ connect_arg<&BaseEngineLogic::RequestDestroyActorDelegate>, this }, EvtData_Request_Destroy_Actor::sk_EventType);
	pGlobalEventManager->VRemoveListener({ connect_arg<&BaseEngineLogic::RequestNewActorDelegate>, this }, EvtData_Request_New_Actor::sk_EventType);
	pGlobalEventManager->VRemoveListener({ connect_arg<&BaseEngineLogic::MoveActorDelegate>, this }, EvtData_Move_Actor::sk_EventType);
	pGlobalEventManager->VRemoveListener({ connect_arg<&BaseEngineLogic::RequestStartGameDelegate>, this }, EvtData_Request_Start_Game::sk_EventType);
	pGlobalEventManager->VRemoveListener({ connect_arg<&BaseEngineLogic::EnvironmentLoadedDelegate>, this }, EvtData_Environment_Loaded::sk_EventType);
	pGlobalEventManager->VRemoveListener({ connect_arg<&BaseEngineLogic::SphereParticleContactDelegate>, this }, EvtData_Sphere_Particle_Contact::sk_EventType);
}
