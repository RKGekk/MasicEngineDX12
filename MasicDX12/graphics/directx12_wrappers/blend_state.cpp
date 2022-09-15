#include "blend_state.h"

#include <cassert>

BlendState::BlendState() {
    for (int rt = 0; rt < 8; ++rt) {
        m_desc.RenderTarget[rt].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        m_desc.RenderTarget[rt].LogicOp = D3D12_LOGIC_OP_NOOP;
        m_desc.RenderTarget[rt].LogicOpEnable = false;
        m_desc.IndependentBlendEnable = true;

        SetSourceValues(Value::One, Value::One, AttachmentPoint(rt));
        SetDestinationValues(Value::Zero, Value::Zero, AttachmentPoint(rt));
        SetFunctions(Function::Addition, Function::Addition, AttachmentPoint(rt));
        SetBlendingEnabled(false, AttachmentPoint(rt));
    }
}

BlendState::BlendState(const D3D12_BLEND_DESC& desc) : m_desc(desc) {}

void BlendState::SetSourceValues(Value color, Value alpha, AttachmentPoint render_target) {
    auto rt = std::underlying_type<AttachmentPoint>::type(render_target);
    m_desc.RenderTarget[rt].SrcBlend = GetValue(color);
    m_desc.RenderTarget[rt].SrcBlendAlpha = GetValue(alpha);
}

void BlendState::SetDestinationValues(Value color, Value alpha, AttachmentPoint render_target) {
    auto rt = std::underlying_type<AttachmentPoint>::type(render_target);
    m_desc.RenderTarget[rt].DestBlend = GetValue(color);
    m_desc.RenderTarget[rt].DestBlendAlpha = GetValue(alpha);
}

void BlendState::SetFunctions(Function color_function, Function alpha_function, AttachmentPoint render_target) {
    D3D12_BLEND_OP color_value{};
    D3D12_BLEND_OP alpha_value{};

    switch (color_function) {
        case Function::Addition: color_value = D3D12_BLEND_OP_ADD; break;
        case Function::Substraction: color_value = D3D12_BLEND_OP_SUBTRACT; break;
        case Function::ReverseSubstraction: color_value = D3D12_BLEND_OP_REV_SUBTRACT; break;
        case Function::Min: color_value = D3D12_BLEND_OP_MIN; break;
        case Function::Max: color_value = D3D12_BLEND_OP_MAX; break;
    }

    switch (alpha_function) {
        case Function::Addition: alpha_value = D3D12_BLEND_OP_ADD; break;
        case Function::Substraction: alpha_value = D3D12_BLEND_OP_SUBTRACT; break;
        case Function::ReverseSubstraction: alpha_value = D3D12_BLEND_OP_REV_SUBTRACT; break;
        case Function::Min: alpha_value = D3D12_BLEND_OP_MIN; break;
        case Function::Max: alpha_value = D3D12_BLEND_OP_MAX; break;
    }

    auto rt = std::underlying_type<AttachmentPoint>::type(render_target);
    m_desc.RenderTarget[rt].BlendOp = color_value;
    m_desc.RenderTarget[rt].BlendOpAlpha = alpha_value;
}

void BlendState::SetBlendingEnabled(bool enabled, AttachmentPoint render_target) {
    auto rt = std::underlying_type<AttachmentPoint>::type(render_target);
    m_desc.RenderTarget[rt].BlendEnable = enabled;
}

const D3D12_BLEND_DESC& BlendState::GetState() const {
	return m_desc;
}

void BlendState::SetState(const D3D12_BLEND_DESC& desc) {
    m_desc = desc;
}

D3D12_BLEND BlendState::GetValue(Value value) {
    switch (value) {
        case Value::Zero: return D3D12_BLEND_ZERO;
        case Value::One: return D3D12_BLEND_ONE;
        case Value::SourceColor: return D3D12_BLEND_SRC_COLOR;
        case Value::InverveSourceColor: return D3D12_BLEND_INV_SRC_COLOR;
        case Value::SourceAlpha: return D3D12_BLEND_SRC_ALPHA;
        case Value::InverseSourceAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
        case Value::DestinationAlpha: return D3D12_BLEND_DEST_ALPHA;
        case Value::InverseDestinationAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
        case Value::DestinationColor: return D3D12_BLEND_DEST_COLOR;
        case Value::InverseDestinationColor: return D3D12_BLEND_INV_DEST_COLOR;
        default: assert(false); return D3D12_BLEND_ONE; //Should never be hit
    }
}
