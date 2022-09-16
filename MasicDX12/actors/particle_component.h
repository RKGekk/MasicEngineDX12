#pragma once

#include <string>
#include <memory>

#include <DirectXMath.h>
#include <pugixml/pugixml.hpp>

#include "actor_component.h"
#include "../engine/i_engine_physics.h"
#include "../physics/particle.h"

using namespace std::literals;

class ParticleComponent : public ActorComponent {
public:
    static const std::string g_Name;
    virtual const std::string& VGetName() const override;

public:
    ParticleComponent();
    virtual ~ParticleComponent();

    virtual pugi::xml_node VGenerateXml() override;

    virtual bool VInit(const pugi::xml_node& data) override;
    virtual void VPostInit() override;
    virtual void VUpdate(const GameTimerDelta& delta) override;

    virtual Particle& VGetParticle();
    virtual Particle* VGetParticlePtr();

protected:
    virtual void VRegisterEvents();

    Particle m_particle;
    std::weak_ptr<IEnginePhysics> m_pGame_physics;
};