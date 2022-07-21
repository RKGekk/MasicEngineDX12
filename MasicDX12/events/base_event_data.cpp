#include "base_event_data.h"

BaseEventData::BaseEventData(const gameTimePoint timeStamp) : m_timeStamp(timeStamp) {}

gameTimePoint BaseEventData::GetTimeStamp() const {
	return m_timeStamp;
}

void BaseEventData::VSerialize(std::ostream& out) const {}

void BaseEventData::VDeserialize(std::istream& in) {}