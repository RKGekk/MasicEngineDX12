#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "../tools/string_utility.h"

class EvtData_Paint : public BaseEventData {
public:
    static const EventTypeId sk_EventType = 0xe5db9f0e;
    static const std::string sk_EventName;

    EvtData_Paint();

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Paint& evt);
};