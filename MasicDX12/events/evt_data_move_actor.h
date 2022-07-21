#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "../actors/actor.h"
#include "../tools/string_utility.h"

class EvtData_Move_Actor : public BaseEventData {
    ActorId m_id;
    DirectX::XMFLOAT4X4 m_matrix;

public:
    static const EventTypeId sk_EventType = 0xeeaa0a40;
    static const std::string sk_EventName;

    EvtData_Move_Actor();
    EvtData_Move_Actor(ActorId id, const DirectX::XMFLOAT4X4& matrix);

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    ActorId GetId() const;
    const DirectX::XMFLOAT4X4& GetMatrix4x4() const;
    DirectX::XMMATRIX GetMatrix() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Move_Actor& evt);
};