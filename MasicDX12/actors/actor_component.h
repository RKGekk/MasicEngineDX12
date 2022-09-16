#pragma once

#include <memory>
#include <functional>

#include <pugixml/pugixml.hpp>

#include "Actor.h"
#include "../tools/game_timer.h"

class ActorComponent {
	friend class ActorFactory;

protected:
	virtual void VRegisterEvents() = 0;

	inline static bool m_events_registered = false;
	StrongActorPtr m_pOwner;

public:
	virtual ~ActorComponent();

	virtual bool VInit(const pugi::xml_node& data) = 0;
	virtual void VPostInit();
	virtual void VUpdate(const GameTimerDelta& delta);
	virtual void VOnChanged();

	virtual pugi::xml_node VGenerateXml() = 0;

	virtual ComponentId VGetId() const;
	virtual const std::string& VGetName() const = 0;
	static ComponentId GetIdFromName(const std::string& componentStr);

	void SetOwner(StrongActorPtr pOwner);
	StrongActorPtr GetOwner();
	ActorId GetOwnerId();
};