#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "../actors/actor.h"
#include "../tools/string_utility.h"

class EvtData_DPI_Scale : public BaseEventData {
    float m_dpi_scale;

public:
    static const EventTypeId sk_EventType = 0x46e618c7;
    static const std::string sk_EventName;

    EvtData_DPI_Scale();
    EvtData_DPI_Scale(float dpi_scale);

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    float GetDPIScale() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_DPI_Scale& evt);
};