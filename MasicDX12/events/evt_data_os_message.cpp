#include "evt_data_os_message.h"

const std::string EvtData_OS_Message::sk_EventName = "EvtData_OS_Message";

EvtData_OS_Message::EvtData_OS_Message() {
	m_data = {0};
}

EvtData_OS_Message::EvtData_OS_Message(OSMessageData data) {
	m_data = data;
}

void EvtData_OS_Message::VDeserialize(std::istream& in) {}

EventTypeId EvtData_OS_Message::VGetEventType() const {
	return sk_EventType;
}

IEventDataPtr EvtData_OS_Message::VCopy() const {
	return IEventDataPtr(new EvtData_OS_Message(m_data));
}

void EvtData_OS_Message::VSerialize(std::ostream& out) const {
	out << m_data.uMsg << " ";
}

const std::string& EvtData_OS_Message::GetName() const {
	return sk_EventName;
}

const OSMessageData EvtData_OS_Message::GetOSMessage() const {
	return m_data;
}

std::ostream& operator<<(std::ostream& os, const EvtData_OS_Message& evt) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "Event type id: " << evt.sk_EventType << std::endl;
	os << "Event name: " << evt.sk_EventName << std::endl;
	os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
	os << "Event message: " << evt.m_data.uMsg << std::endl;
	os.flags(oldFlag);
	return os;
}