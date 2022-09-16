#include "particle_component.h"
#include "../engine/engine.h"
#include "transform_component.h"
#include "../events/evt_data_new_particle_component.h"
#include "../events/i_event_manager.h"
#include "../tools/memory_utility.h"

#include <algorithm>
#include <functional>
#include <cctype>

const std::string ParticleComponent::g_Name = "ParticleComponent"s;

const std::string& ParticleComponent::VGetName() const {
    return ParticleComponent::g_Name;
}

ParticleComponent::ParticleComponent() {
    m_particle.setPosition(0.0f, 0.0f, 0.0f);
    m_particle.setVelocity(0.0f, 0.0f, 0.0f);
    m_particle.setAcceleration(0.0f, 0.0f, 0.0f);
    m_particle.setMass(1.0f);
    m_particle.setDamping(0.9f);
    m_particle.setRadius(1.0f);
    m_particle.setCanSleep(true);
    m_particle.setAwake(true);
}

ParticleComponent::~ParticleComponent() {
    if (auto phys = m_pGame_physics.lock()) {
        phys->VRemoveParticle(&m_particle);
    }
}

pugi::xml_node ParticleComponent::VGenerateXml() {
    return pugi::xml_node();
}

void ParticleComponent::VRegisterEvents() {}

bool ParticleComponent::VInit(const pugi::xml_node& data) {
    m_pGame_physics = Engine::GetEngine()->GetGameLogic()->VGetGamePhysics();
    if (m_pGame_physics.expired()) return false;

    m_particle.setVelocity3f(posfromattr3f(data.child("Velocity")));
    m_particle.setAcceleration3f(posfromattr3f(data.child("Acceleration")));
    m_particle.setMass(data.child("Acceleration").text().as_float(m_particle.getMass()));
    m_particle.setDamping(data.child("Damping").text().as_float(m_particle.getDamping()));
    m_particle.setRadius(data.child("Radius").text().as_float(m_particle.getRadius()));
    m_particle.setAwake(data.child("IsSleep").text().as_bool(m_particle.getAwake()));
    m_particle.setCanSleep(data.child("CanSleep").text().as_bool(m_particle.getCanSleep()));

    return true;
}

void ParticleComponent::VPostInit() {
    std::shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent) {
        m_particle.setPosition3f(pTransformComponent->GetPosition3f());
    }

    std::shared_ptr<EvtData_New_Particle_Component> pEvent(new EvtData_New_Particle_Component(m_pOwner->GetId(), &m_particle));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void ParticleComponent::VUpdate(const GameTimerDelta& delta) {}

Particle& ParticleComponent::VGetParticle() {
    return m_particle;
}

Particle* ParticleComponent::VGetParticlePtr() {
    return &m_particle;
}
