#pragma once

#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "..//actors/actor.h"

class EvtData_Sphere_Particle_Contact : public BaseEventData {
	ActorId m_actorId_1;
	ActorId m_actorId_2;

public:
	static const EventTypeId sk_EventType = 0xbd210ebb;
	static const std::string sk_EventName;

	EvtData_Sphere_Particle_Contact();
	explicit EvtData_Sphere_Particle_Contact(ActorId actorId_1, ActorId actorId_2);

	virtual void VDeserialize(std::istream& in) override;
	virtual EventTypeId VGetEventType() const override;
	virtual IEventDataPtr VCopy() const override;
	virtual void VSerialize(std::ostream& out) const override;
	virtual const std::string& GetName() const override;

	const ActorId GetActorId1() const;
	const ActorId GetActorId2() const;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_Sphere_Particle_Contact& evt);
};