#include "evt_data_request_new_actor.h"
#include "../tools/string_utility.h"

const std::string EvtData_Request_New_Actor::sk_EventName = "EvtData_Request_New_Actor";

EvtData_Request_New_Actor::EvtData_Request_New_Actor() {
	m_actorResource = "";
	m_hasInitialTransform = false;
	DirectX::XMStoreFloat4x4(&m_initialTransform, DirectX::XMMatrixIdentity());
	m_serverActorId = -1;
	m_viewId = 0;
}

EvtData_Request_New_Actor::EvtData_Request_New_Actor(const std::string& actorResource, const DirectX::XMFLOAT4X4* initialTransform, const ActorId serverActorId, const EngineViewId viewId) {
	m_actorResource = actorResource;
	if (initialTransform) {
		m_hasInitialTransform = true;
		m_initialTransform = *initialTransform;
	}
	else {
		m_hasInitialTransform = false;
	}

	m_serverActorId = serverActorId;
	m_viewId = viewId;
}

EvtData_Request_New_Actor::EvtData_Request_New_Actor(const std::string& actorResource, const DirectX::FXMMATRIX initialTransform, const ActorId serverActorId, const EngineViewId viewId) {
	m_actorResource = actorResource;

	m_hasInitialTransform = true;
	DirectX::XMStoreFloat4x4(&m_initialTransform, initialTransform);

	m_serverActorId = serverActorId;
	m_viewId = viewId;
}

EventTypeId EvtData_Request_New_Actor::VGetEventType() const {
	return sk_EventType;
}

void EvtData_Request_New_Actor::VDeserialize(std::istream& in) {
	in >> m_actorResource;
	in >> m_hasInitialTransform;
	if (m_hasInitialTransform) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				in >> m_initialTransform.m[i][j];
			}
		}
	}
	in >> m_serverActorId;
}

IEventDataPtr EvtData_Request_New_Actor::VCopy() const {
	return IEventDataPtr(new EvtData_Request_New_Actor(m_actorResource, (m_hasInitialTransform) ? &m_initialTransform : NULL, m_serverActorId));
}

void EvtData_Request_New_Actor::VSerialize(std::ostream& out) const {
	out << m_actorResource << " ";
	out << m_hasInitialTransform << " ";
	if (m_hasInitialTransform) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				out << m_initialTransform.m[i][j] << " ";
			}
		}
	}
	out << m_serverActorId << " ";
}

const std::string& EvtData_Request_New_Actor::GetName() const {
	return sk_EventName;
}

const std::string& EvtData_Request_New_Actor::GetActorResource() const {
	return m_actorResource;
}

const DirectX::XMFLOAT4X4* EvtData_Request_New_Actor::GetInitialTransform() const {
	return (m_hasInitialTransform) ? &m_initialTransform : NULL;
}

const ActorId EvtData_Request_New_Actor::GetServerActorId() const {
	return m_serverActorId;
}

unsigned int EvtData_Request_New_Actor::GetViewId() const {
	return m_viewId;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Request_New_Actor& evt) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "Event type id: " << evt.sk_EventType << std::endl;
	os << "Event name: " << evt.sk_EventName << std::endl;
	os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
	os << "Event server actorId id: " << evt.m_serverActorId << std::endl;
	os << "Event actor resource: " << evt.m_actorResource << std::endl;
	os << "Event has initial transform: " << evt.m_hasInitialTransform << std::endl;
	os << "Event initial transform matrix: " << std::endl << evt.m_initialTransform << std::endl;

	os.flags(oldFlag);
	return os;
}