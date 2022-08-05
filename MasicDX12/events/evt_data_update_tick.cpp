#include "evt_data_update_tick.h"

const std::string EvtData_Update_Tick::sk_EventName = "EvtData_Update_Tick";

EvtData_Update_Tick::EvtData_Update_Tick() : GameTimerDelta(ZERO_DURATION, ZERO_DURATION) {}

EvtData_Update_Tick::EvtData_Update_Tick(GameClockDuration delta_time, GameClockDuration total_time) : GameTimerDelta(delta_time, total_time) {}

EvtData_Update_Tick::EvtData_Update_Tick(GameTimerDelta delta_time) : GameTimerDelta(delta_time) {}

EventTypeId EvtData_Update_Tick::VGetEventType() const {
    return sk_EventType;
}

IEventDataPtr EvtData_Update_Tick::VCopy() const {
    return IEventDataPtr(new EvtData_Update_Tick(m_delta_time_duration, m_total_time));
}

void EvtData_Update_Tick::VSerialize(std::ostream& out) const {}

const std::string& EvtData_Update_Tick::GetName() const {
    return sk_EventName;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Update_Tick& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event delta time ns: " << evt.GetDeltaNanoseconds() << std::endl;
    os << "Event total time ms: " << evt.fGetTotalMilliSeconds() << std::endl;
    os.flags(oldFlag);
    return os;
}
