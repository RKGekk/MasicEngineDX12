#include "base_event_data.h"

BaseEventData::BaseEventData(const GameTimer::gameTimePoint time_stamp) : m_time_stamp(time_stamp) {}

GameTimer::gameTimePoint BaseEventData::GetTimeStamp() const {
	return m_time_stamp;
}

void BaseEventData::VSerialize(std::ostream& out) const {}

void BaseEventData::VDeserialize(std::istream& in) {}