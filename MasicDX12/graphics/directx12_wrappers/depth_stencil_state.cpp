#include "depth_stencil_state.h"

DepthStencilState::DepthStencilState() {
    SetDepthTestEnabled(true);
    SetDepthWriteEnabled(true);
    SetComparisonFunction(ComparisonFunction::LessOrEqual);

    m_desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    m_desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    m_desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    m_desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

    m_desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    m_desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    m_desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    m_desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

    m_desc.StencilEnable = false;
}

DepthStencilState::DepthStencilState(D3D12_DEPTH_STENCIL_DESC desc) : m_desc(desc) {}

void DepthStencilState::SetDepthTestEnabled(bool enabled) {
    m_desc.DepthEnable = enabled;
}

void DepthStencilState::SetDepthWriteEnabled(bool enabled) {
    m_desc.DepthWriteMask = enabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
}

void DepthStencilState::SetComparisonFunction(ComparisonFunction function) {
    switch (function) {
        case ComparisonFunction::Never: m_desc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER; break;
        case ComparisonFunction::Less: m_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; break;
        case ComparisonFunction::Equal: m_desc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL; break;
        case ComparisonFunction::LessOrEqual: m_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
        case ComparisonFunction::Greater: m_desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER; break;
        case ComparisonFunction::NotEqual: m_desc.DepthFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL; break;
        case ComparisonFunction::GreaterOrEqual: m_desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; break;
        case ComparisonFunction::Always: m_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; break;
    }
}

const D3D12_DEPTH_STENCIL_DESC& DepthStencilState::GetState() const {
	return m_desc;
}
