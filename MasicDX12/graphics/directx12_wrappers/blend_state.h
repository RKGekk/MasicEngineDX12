#pragma once

#include <d3d12.h>

#include "render_target.h"

class BlendState {
public:
    enum class Value { Zero, One, SourceColor, InverveSourceColor, SourceAlpha, InverseSourceAlpha, DestinationAlpha, InverseDestinationAlpha, DestinationColor, InverseDestinationColor };
    enum class Function { Addition, Substraction, ReverseSubstraction, Min, Max };

    BlendState();
    BlendState(const D3D12_BLEND_DESC& desc);

    void SetSourceValues(Value color, Value alpha, AttachmentPoint render_target = AttachmentPoint::Color0);
    void SetDestinationValues(Value color, Value alpha, AttachmentPoint render_target = AttachmentPoint::Color0);
    void SetFunctions(Function color_function, Function alpha_function, AttachmentPoint render_target = AttachmentPoint::Color0);
    void SetBlendingEnabled(bool enabled, AttachmentPoint render_target = AttachmentPoint::Color0);

    const D3D12_BLEND_DESC& GetState() const;
    void SetState(const D3D12_BLEND_DESC& desc);

private:
    D3D12_BLEND GetValue(Value value);

    D3D12_BLEND_DESC m_desc{};
};