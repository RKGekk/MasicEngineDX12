#pragma once

#include <memory>

#include "../tools/delegate.h"
#include "../tools/game_timer.h"

class IEventData;

using EventTypeId = unsigned long;
using IEventDataPtr = std::shared_ptr<IEventData>;
using EventListenerDelegate = delegate<void(IEventDataPtr)>;

class IEventData {
public:
	virtual ~IEventData() = default;

	virtual EventTypeId VGetEventType() const = 0;
	virtual gameTimePoint GetTimeStamp() const = 0;
	virtual void VSerialize(std::ostream& out) const = 0;
	virtual void VDeserialize(std::istream& in) = 0;
	virtual IEventDataPtr VCopy(void) const = 0;
	virtual const std::string& GetName() const = 0;
};