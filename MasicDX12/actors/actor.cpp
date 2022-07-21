#include "actor.h"
#include "actor_component.h"

Actor::Actor(ActorId id) {
    m_id = id;
    m_type_name = "Unknown";
    m_resource_name = "Unknown";
}

Actor::~Actor() {}

bool Actor::Init(const pugi::xml_node& data) {
    m_name = "NoName";
    m_type_name = data.attribute("type").value();
    m_resource_name = data.attribute("resource").value();
    return true;
}

void Actor::PostInit() {
    for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        it->second->VPostInit();
    }
}

void Actor::Destroy() {
    m_components.clear();
}

void Actor::Update(float deltaMs) {
    for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        it->second->VUpdate(deltaMs);
    }
}

ActorId Actor::GetId() const {
    return m_id;
}

const std::string& Actor::GetType() const {
    return m_type_name;
}

const std::string& Actor::GetName() const {
    return m_name;
}

void Actor::SetName(std::string new_name) {
    m_name = std::move(new_name);
}

const ActorComponents& Actor::GetComponents() {
    return m_components;
}

void Actor::AddComponent(StrongActorComponentPtr pComponent) {
    std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetId(), pComponent));
}

std::string Actor::ToXML() {
    return "";
}