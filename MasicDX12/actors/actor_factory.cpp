#include "actor_factory.h"

#include "transform_component.h"
#include "mesh_component.h"
#include "particle_component.h"
#include "orientation_relation_component.h"
#include "enemy_component.h"
#include "camera_component.h"
#include "light_component.h"
#include "shadow_camera_component.h"

unsigned int ActorFactory::GetNextActorId() {
    return ++m_last_actorId;
}

ActorFactory::ActorFactory() {
    m_last_actorId = 0;

    m_component_factory.Register<TransformComponent>(ActorComponent::GetIdFromName(TransformComponent::g_Name), TransformComponent::g_Name);
    m_component_factory.Register<MeshComponent>(ActorComponent::GetIdFromName(MeshComponent::g_Name), MeshComponent::g_Name);
    m_component_factory.Register<ParticleComponent>(ActorComponent::GetIdFromName(ParticleComponent::g_Name), ParticleComponent::g_Name);
    m_component_factory.Register<OrientationRelationComponent>(ActorComponent::GetIdFromName(OrientationRelationComponent::g_Name), OrientationRelationComponent::g_Name);
    m_component_factory.Register<EnemyComponent>(ActorComponent::GetIdFromName(EnemyComponent::g_Name), EnemyComponent::g_Name);
    m_component_factory.Register<CameraComponent>(ActorComponent::GetIdFromName(CameraComponent::g_Name), CameraComponent::g_Name);
    m_component_factory.Register<ShadowCameraComponent>(ActorComponent::GetIdFromName(ShadowCameraComponent::g_Name), ShadowCameraComponent::g_Name);
    m_component_factory.Register<LightComponent>(ActorComponent::GetIdFromName(LightComponent::g_Name), LightComponent::g_Name);
}

std::shared_ptr<Actor> ActorFactory::CreateActor(const std::string& actor_resource, const pugi::xml_node& overrides, const DirectX::XMFLOAT4X4* pInitial_transform, const ActorId servers_actorId) {

    pugi::xml_document xml_doc;
    pugi::xml_parse_result parse_res = xml_doc.load_file(actor_resource.c_str());
    if (!parse_res) return std::shared_ptr<Actor>();

    pugi::xml_node root_node = xml_doc.root();
    if (!root_node) return std::shared_ptr<Actor>();

    pugi::xml_node actor_node = root_node.child("Actor");
    if (!actor_node) return std::shared_ptr<Actor>();

    ActorId next_actorId = servers_actorId;
    if (next_actorId == 0) {
        next_actorId = GetNextActorId();
    }
    std::shared_ptr<Actor> pActor(new Actor(next_actorId));
    if (!pActor->Init(actor_node)) {
        return std::shared_ptr<Actor>();
    }

    bool initial_transform_set = false;
    for (pugi::xml_node node = actor_node.first_child(); node; node = node.next_sibling()) {
        std::shared_ptr<ActorComponent> pComponent(VCreateComponent(node));
        if (pComponent) {
            pActor->AddComponent(pComponent);
            pComponent->SetOwner(pActor);
        }
        else {
            return std::shared_ptr<Actor>();
        }
    }

    if (overrides) {
        ModifyActor(pActor, overrides);
    }

    std::shared_ptr<TransformComponent> pTransform_component = MakeStrongPtr(pActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pInitial_transform && pTransform_component) {
        pTransform_component->SetPosition4x4f(*pInitial_transform);
    }

    pActor->PostInit();

    return pActor;
}

std::shared_ptr<ActorComponent> ActorFactory::VCreateComponent(const pugi::xml_node& data) {
    const char* name = data.name();
    std::shared_ptr<ActorComponent> pComponent(m_component_factory.Create(ActorComponent::GetIdFromName(name)));

    if (pComponent) {
        if (!pComponent->VInit(data)) {
            return std::shared_ptr<ActorComponent>();
        }
    }
    else {
        return std::shared_ptr<ActorComponent>();
    }

    return pComponent;
}

std::shared_ptr<Actor> ActorFactory::CreateActor(const std::string& actor_resource, const pugi::xml_node& overrides, DirectX::FXMMATRIX initial_transform, const ActorId servers_actorId) {
    DirectX::XMFLOAT4X4 T;
    DirectX::XMStoreFloat4x4(&T, initial_transform);
    return CreateActor(actor_resource.c_str(), overrides, &T, servers_actorId);
}

void ActorFactory::ModifyActor(std::shared_ptr<Actor> pActor, const pugi::xml_node& overrides) {
    if (overrides.attribute("name")) {
        pActor->SetName(overrides.attribute("name").as_string());
    };
    for (pugi::xml_node node = overrides.first_child(); node; node = node.next_sibling()) {
        unsigned int componentId = ActorComponent::GetIdFromName(node.name());
        std::shared_ptr<ActorComponent> pComponent = MakeStrongPtr(pActor->GetComponent<ActorComponent>(componentId));
        if (pComponent) {
            pComponent->VInit(node);
            pComponent->VOnChanged();
        }
        else {
            pComponent = VCreateComponent(node);
            if (pComponent) {
                pActor->AddComponent(pComponent);
                pComponent->SetOwner(pActor);
            }
        }
    }
}