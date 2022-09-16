#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>

#include <DirectXMath.h>

#include "../actors/actor.h"
#include "level_manager.h"
#include "i_engine_logic.h"
#include "../processes/process_manager.h"
#include "../tools/game_timer.h"
#include "base_engine_state.h"
#include "../tools/mt_random.h"
#include "i_engine_view.h"
#include "i_engine_physics.h"
#include "../events/i_event_data.h"
#include "human_view.h"

class ActorFactory;
class LevelManager;

class BaseEngineLogic : IEngineLogic {
	friend class Engine;

public:
	BaseEngineLogic();
	virtual ~BaseEngineLogic();
	bool Init();

	ActorId GetNewActorID();

	MTRandom& GetRNG();

	virtual void VAddView(std::shared_ptr<IEngineView> pView, ActorId actorId = INVALID_ACTOR_ID);
	virtual void VRemoveView(std::shared_ptr<IEngineView> pView);

	virtual StrongActorPtr VCreateActor(const std::string& actor_resource, const pugi::xml_node& overrides, DirectX::FXMMATRIX initial_transform, const ActorId servers_actorId = INVALID_ACTOR_ID) override;
	virtual StrongActorPtr VCreateActor(const std::string& actor_resource, const pugi::xml_node& overrides, const DirectX::XMFLOAT4X4* initial_transform, const ActorId servers_actorId = INVALID_ACTOR_ID) override;
	virtual void VDestroyActor(const ActorId actorId) override;

	virtual WeakActorPtr VGetActor(const ActorId actorId) override;
	virtual WeakActorPtr VGetActorByName(const std::string& actor_name);
	virtual const std::unordered_set<ActorId>& VGetActorsByComponent(ComponentId cid);
	virtual bool VCheckActorsExistByComponent(ComponentId cid);

	virtual void VModifyActor(const ActorId actorId, const pugi::xml_node& overrides);
	virtual void VMoveActor(const ActorId id, DirectX::FXMMATRIX mat) override;
	virtual void VMoveActor4x4f(const ActorId id, const DirectX::XMFLOAT4X4& mat) override;

	std::string GetActorXml(const ActorId id);

	std::shared_ptr<CameraNode> GetActiveCamera();

	const LevelManager& GetLevelManager();
	virtual std::shared_ptr<IEnginePhysics> VGetGamePhysics() override;
	virtual bool VLoadGame(const std::string& level_resource) override;
	virtual bool VLoadGame(const std::string& level_resource, std::shared_ptr<HumanView> pHuman_view);
	virtual void VOnUpdate(const GameTimerDelta& delta) override;
	virtual void VChangeState(BaseEngineState new_state) override;
	const BaseEngineState GetState() const;

	std::shared_ptr<HumanView> GetHumanView();
	std::shared_ptr<HumanView> GetHumanViewByName(std::string name);

	void AttachProcess(StrongProcessPtr pProcess);

	void RequestDestroyActorDelegate(IEventDataPtr pEventData);
	void MoveActorDelegate(IEventDataPtr pEventData);
	void RequestNewActorDelegate(IEventDataPtr pEventData);
	void RequestStartGameDelegate(IEventDataPtr pEventData);
	void EnvironmentLoadedDelegate(IEventDataPtr pEventData);
	void SphereParticleContactDelegate(IEventDataPtr pEventData);

protected:
	virtual std::unique_ptr<ActorFactory> VCreateActorFactory();
	virtual bool VLoadGameDelegate(const pugi::xml_node& pLevelData);

	void RegisterAllDelegates();
	virtual void VRegisterEvents();
	void RemoveAllDelegates();


	using ActorMap = std::unordered_map<ActorId, StrongActorPtr>;
	using ComponentsMap = std::unordered_map<ComponentId, std::unordered_set<ActorId>>;
	using ActorNamesMap = std::unordered_map<std::string, StrongActorPtr>;

	GameClockDuration m_life_time;

	GameViewList m_game_views;
	std::unique_ptr<ProcessManager> m_process_manager;
	std::unique_ptr<ActorFactory> m_actor_factory;
	std::shared_ptr<IEnginePhysics> m_physics;
	std::unique_ptr<LevelManager> m_level_manager;
	MTRandom m_random;
	ActorMap m_actors;
	ComponentsMap m_components;
	ActorNamesMap m_actors_names;
	ActorId m_last_actor_id;
	BaseEngineState m_state;
	std::shared_ptr<CameraNode> m_active_camera;

	int m_expected_players;
	int m_human_players_attached;
	int m_human_games_loaded;
	bool m_render_diagnostics;
};