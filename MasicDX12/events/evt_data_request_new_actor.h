#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "../actors/actor.h"
#include "../engine/i_engine_view.h"

class EvtData_Request_New_Actor : public BaseEventData {
	std::string m_actorResource;
	bool m_hasInitialTransform;
	DirectX::XMFLOAT4X4 m_initialTransform;
	ActorId m_serverActorId;
	EngineViewId m_viewId;

public:
	static const EventTypeId sk_EventType = 0x40378c64;
	static const std::string sk_EventName;

	EvtData_Request_New_Actor();
	explicit EvtData_Request_New_Actor(const std::string& actorResource, const DirectX::XMFLOAT4X4* initialTransform = nullptr, const ActorId serverActorId = 0, const EngineViewId viewId = 0);
	explicit EvtData_Request_New_Actor(const std::string& actorResource, const DirectX::FXMMATRIX initialTransform, const ActorId serverActorId = 0, const EngineViewId viewId = 0);

	virtual EventTypeId VGetEventType() const override;
	virtual void VDeserialize(std::istream& in) override;
	virtual IEventDataPtr VCopy() const override;
	virtual void VSerialize(std::ostream& out) const override;
	virtual const std::string& GetName() const override;

	const std::string& GetActorResource() const;
	const DirectX::XMFLOAT4X4* GetInitialTransform() const;
	const ActorId GetServerActorId() const;
	unsigned int GetViewId() const;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_Request_New_Actor& evt);
};