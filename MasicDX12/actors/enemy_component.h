#pragma once

#include <filesystem>
#include <string>

#include <DirectXMath.h>

#include "actor_component.h"
#include "../tools/mt_random.h"
#include "../tools/game_timer.h"

using namespace std::literals;

class EnemyComponent : public ActorComponent {
public:
	static const std::string g_Name;
	virtual const std::string& VGetName() const override;

	EnemyComponent();
	~EnemyComponent();

	virtual bool VInit(const pugi::xml_node& data) override;
	virtual void VPostInit() override;
	virtual void VUpdate(const GameTimerDelta& delta) override;

	const std::string& GetTargetName();

	virtual pugi::xml_node VGenerateXml() override;

protected:
	virtual void VRegisterEvents();

	std::string m_target_name;
	float m_force;
};