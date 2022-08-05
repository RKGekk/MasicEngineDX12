#pragma once

#include "i_event_data.h"
#include "../tools/game_timer.h"

class BaseEventData : public IEventData {
	const GameTimePoint m_time_stamp;

public:
	explicit BaseEventData(const GameTimePoint timeStamp = GameClock::now());

	GameTimePoint GetTimeStamp() const override;

	virtual void VSerialize(std::ostream& out) const override;
	virtual void VDeserialize(std::istream& in) override;
};