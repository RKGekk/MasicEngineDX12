#pragma once

#include "i_event_data.h"
#include "../tools/game_timer.h"

class BaseEventData : public IEventData {
	const gameTimePoint m_timeStamp;

public:
	explicit BaseEventData(const gameTimePoint timeStamp = gameClock::now());

	gameTimePoint GetTimeStamp() const override;

	virtual void VSerialize(std::ostream& out) const override;
	virtual void VDeserialize(std::istream& in) override;
};