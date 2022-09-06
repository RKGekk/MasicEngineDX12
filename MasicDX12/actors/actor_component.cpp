#include "actor_component.h"

#include "actor.h"

ActorComponent::~ActorComponent() {
	m_pOwner.reset();
}

void ActorComponent::VPostInit() {}

void ActorComponent::VUpdate(const GameTimerDelta& delta) {}

void ActorComponent::VOnChanged() {}

ComponentId ActorComponent::GetIdFromName(const std::string& componentStr) {
	return static_cast<ComponentId>(std::hash<std::string>{}(componentStr));
}

ComponentId ActorComponent::VGetId() const {
	return GetIdFromName(VGetName());
}

void ActorComponent::SetOwner(StrongActorPtr pOwner) {
	m_pOwner = pOwner;
}

StrongActorPtr ActorComponent::GetOwner() {
	return m_pOwner;
}

ActorId ActorComponent::GetOwnerId() {
	return m_pOwner->GetId();
}