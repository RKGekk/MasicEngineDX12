#include "enemy_component.h"
#include "transform_component.h"
#include "particle_component.h"
#include "../tools/memory_utility.h"
#include "../engine/engine.h"

const std::string EnemyComponent::g_Name = "EnemyComponent"s;

const std::string& EnemyComponent::VGetName() const {
	return EnemyComponent::g_Name;
}

EnemyComponent::EnemyComponent() {
	m_target_name = "NoTarget";
	m_force = 100.0f;
}

EnemyComponent::~EnemyComponent() {}

bool EnemyComponent::VInit(const pugi::xml_node& data) {
	pugi::xml_node enemy_target_node = data.child("EnemyTarget");
	if (enemy_target_node) {
		m_target_name = enemy_target_node.value();
	}

	m_force = data.child("EnemyForce").text().as_float(m_force);

	return true;
}

void EnemyComponent::VPostInit() {}

void EnemyComponent::VUpdate(const GameTimerDelta& delta) {
	float deltaMs = delta.fGetDeltaMilliseconds();
	using namespace DirectX;
	if (Engine::GetEngine()->GetGameLogic()->GetState() != BaseEngineState::BGS_Running) { return; }
	StrongActorPtr pTargetActor = Engine::GetEngine()->GetGameLogic()->VGetActorByName(m_target_name).lock();
	if (!pTargetActor) { return; }

	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<TransformComponent> tc = MakeStrongPtr(act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")));
	std::shared_ptr<ParticleComponent> pc = MakeStrongPtr(act->GetComponent<ParticleComponent>(ActorComponent::GetIdFromName("ParticleComponent")));
	std::shared_ptr<TransformComponent> target_tc = MakeStrongPtr(pTargetActor->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")));
	XMVECTOR direction = XMVector3Normalize(target_tc->GetPosition() - tc->GetPosition());
	if (pc) {
		Particle& particle = pc->VGetParticle();
		particle.clearAccumulator();
		particle.addForce(direction * m_force);
		particle.integrate(deltaMs);
	}
	else {
		tc->SetPosition3(tc->GetPosition() + direction * deltaMs);
	}
}

const std::string& EnemyComponent::GetTargetName() {
	return m_target_name;
}

pugi::xml_node EnemyComponent::VGenerateXml() {
	return pugi::xml_node();
}

void EnemyComponent::VRegisterEvents() {}
