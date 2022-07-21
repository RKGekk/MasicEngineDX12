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
#include "base_engine_state.h"
#include "../tools/mt_random.h"
#include "i_engine_view.h"
#include "i_engine_physics.h"
#include "../events/i_event_data.h"
#include "human_view.h"

class ActorFactory;
class LevelManager;

typedef std::unordered_map<ActorId, StrongActorPtr> ActorMap;
typedef std::unordered_map<ComponentId, std::unordered_set<ActorId>> ComponentsMap;

class BaseEngineLogic : IEngineLogic {
	friend class Engine;

protected:
	float m_life_time;

	GameViewList m_game_views;
	std::unique_ptr<ProcessManager> m_process_manager;
	std::unique_ptr<ActorFactory> m_actor_factory;
	std::unique_ptr<IEnginePhysics> m_physics;
	std::unique_ptr<LevelManager> m_level_manager;
	MTRandom m_random;
	ActorMap m_actors;
	ComponentsMap m_components;
	std::unordered_map<std::string, StrongActorPtr> m_actors_names;
	ActorId m_last_actor_id;
	BaseEngineState m_state;

	int m_expected_players;
	int m_human_players_attached;
	int m_human_games_loaded;
	bool m_render_diagnostics;

public:

	BaseEngineLogic();
	virtual ~BaseEngineLogic();
	bool Init();

	ActorId GetNewActorID();

	MTRandom& GetRNG();

	virtual void VAddView(std::shared_ptr<IEngineView> pView, ActorId actorId = INVALID_ACTOR_ID);
	virtual void VRemoveView(std::shared_ptr<IEngineView> pView);

	virtual StrongActorPtr VCreateActor(const std::string& actorResource, const pugi::xml_node& overrides, DirectX::FXMMATRIX initialTransform, const ActorId serversActorId = INVALID_ACTOR_ID) override;
	virtual StrongActorPtr VCreateActor(const std::string& actorResource, const pugi::xml_node& overrides, const DirectX::XMFLOAT4X4* initialTransform, const ActorId serversActorId = INVALID_ACTOR_ID) override;
	virtual void VDestroyActor(const ActorId actorId) override;
	virtual WeakActorPtr VGetActor(const ActorId actorId) override;
	virtual WeakActorPtr VGetActorByName(const std::string& actor_name);
	virtual const std::unordered_set<ActorId>& VGetActorsByComponent(ComponentId cid);
	virtual bool VCheckActorsExistByComponent(ComponentId cid);
	virtual void VModifyActor(const ActorId actorId, const pugi::xml_node& overrides);
	virtual void VMoveActor(const ActorId id, DirectX::FXMMATRIX mat) override;
	virtual void VMoveActor4x4f(const ActorId id, const DirectX::XMFLOAT4X4& mat) override;

	std::string GetActorXml(const ActorId id);

	const LevelManager* GetLevelManager();
	virtual IEnginePhysics* VGetGamePhysics() override;
	virtual bool VLoadGame(const char* levelResource) override;
	virtual bool VLoadGame(const char* levelResource, std::shared_ptr<HumanView> hv);
	virtual void VOnUpdate(float time, float elapsedTime) override;
	virtual void VChangeState(BaseEngineState newState) override;
	const BaseEngineState GetState() const;

	void AttachProcess(StrongProcessPtr pProcess);
	void RequestDestroyActorDelegate(IEventDataPtr pEventData);

protected:
	virtual std::unique_ptr<ActorFactory> VCreateActorFactory();
	virtual bool VLoadGameDelegate(const pugi::xml_node& pLevelData);

	void MoveActorDelegate(IEventDataPtr pEventData);
	void RequestNewActorDelegate(IEventDataPtr pEventData);
};