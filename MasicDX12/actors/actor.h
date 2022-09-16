#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <string>
#include <utility>

#include <pugixml/pugixml.hpp>
#include "../tools/game_timer.h"

class Actor;
class ActorComponent;
class SceneNode;

using ActorId = unsigned int;
using ComponentId = unsigned int;

const ActorId INVALID_ACTOR_ID = 0;
const ComponentId INVALID_COMPONENT_ID = 0;

using StrongActorPtr = std::shared_ptr<Actor>;
using WeakActorPtr = std::weak_ptr<Actor>;
using StrongActorComponentPtr = std::shared_ptr<ActorComponent>;
using WeakActorComponentPtr = std::weak_ptr<ActorComponent>;
using ActorComponents = std::unordered_map<ComponentId, StrongActorComponentPtr>;

struct actor_component_hash {
    std::size_t operator () (const std::pair<ActorId, ComponentId>& p) const {
        return std::hash<ActorId>{}(p.first) ^ p.second;
        //return (p.first << 16U) + p.second;
    }
};

typedef std::unordered_map<std::pair<ActorId, ComponentId>, std::shared_ptr<SceneNode>, actor_component_hash> SceneActorMap;
typedef std::unordered_map<ActorId, std::unordered_set<ComponentId>> ActorComponentMap;

class Actor {
private:
    std::string m_resource_name;
    inline static bool m_events_registered = false;

    ActorId m_id;
    std::string m_type_name;
    std::string m_name;
    ActorComponents m_components;

public:
    explicit Actor(ActorId id);
    ~Actor();

    bool Init(const pugi::xml_node& data);
    void PostInit();
    void Destroy();
    void Update(const GameTimerDelta& delta);

    std::string ToXML();

    unsigned int GetId() const;
    const std::string& GetType() const;
    const std::string& GetName() const;
    void SetName(std::string new_name);

    template <class ComponentType>
    std::weak_ptr<ComponentType> GetComponent(ComponentId id) {
        auto findIt = m_components.find(id);
        if (findIt != m_components.end()) {
            StrongActorComponentPtr pBase(findIt->second);
            std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));
            std::weak_ptr<ComponentType> pWeakSub(pSub);
            return pWeakSub;
        }
        else {
            return std::weak_ptr<ComponentType>();
        }
    }

    template <class ComponentType>
    std::weak_ptr<ComponentType> GetComponent(const char* name) {
        unsigned int id = ActorComponent::GetIdFromName(name);
        auto findIt = m_components.find(id);
        if (findIt != m_components.end()) {
            StrongActorComponentPtr pBase(findIt->second);
            std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));
            std::weak_ptr<ComponentType> pWeakSub(pSub);
            return pWeakSub;
        }
        else {
            return std::weak_ptr<ComponentType>();
        }
    }

    template <class ComponentType>
    std::weak_ptr<ComponentType> GetComponent() {
        unsigned int id = ActorComponent::GetIdFromName(ComponentType::g_Name.c_str());
        auto findIt = m_components.find(id);
        if (findIt != m_components.end()) {
            StrongActorComponentPtr pBase(findIt->second);
            std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));
            std::weak_ptr<ComponentType> pWeakSub(pSub);
            return pWeakSub;
        }
        else {
            return std::weak_ptr<ComponentType>();
        }
    }

    template <class ComponentType>
    std::weak_ptr<ComponentType> GetComponent(const std::string& name) {
        return GetComponent<ComponentType>(name.c_str());
    }

    const ActorComponents& GetComponents();
    void AddComponent(StrongActorComponentPtr pComponent);

protected:
    virtual void VRegisterEvents();
};