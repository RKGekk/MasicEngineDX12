#include "evt_data_paint.h"

const std::string EvtData_Paint::sk_EventName = "EvtData_Paint";

EventTypeId EvtData_Paint::VGetEventType() const {
    return sk_EventType;
}

EvtData_Paint::EvtData_Paint() {}

void EvtData_Paint::VSerialize(std::ostream& out) const {}

void EvtData_Paint::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Paint::VCopy() const {
    return IEventDataPtr(new EvtData_Paint());
}

const std::string& EvtData_Paint::GetName() const {
    return sk_EventName;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Paint& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os.flags(oldFlag);
    return os;
}