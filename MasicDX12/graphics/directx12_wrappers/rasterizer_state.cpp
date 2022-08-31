#include "rasterizer_state.h"

RasterizerState::RasterizerState() {
    SetCullMode(CullMode::Back);
    SetFillMode(FillMode::Solid);
    SetFrontClockwise(true);

    m_desc.DepthClipEnable = true;
}

RasterizerState::RasterizerState(const D3D12_RASTERIZER_DESC& desc) : m_desc(desc) {}

void RasterizerState::SetFillMode(FillMode mode) {
    switch (mode) {
        case FillMode::Wireframe: m_desc.FillMode = D3D12_FILL_MODE_WIREFRAME; break;
        case FillMode::Solid: m_desc.FillMode = D3D12_FILL_MODE_SOLID; break;
    }
}

void RasterizerState::SetCullMode(CullMode mode) {
    switch (mode) {
        case CullMode::None: m_desc.CullMode = D3D12_CULL_MODE_NONE; break;
        case CullMode::Front: m_desc.CullMode = D3D12_CULL_MODE_FRONT; break;
        case CullMode::Back: m_desc.CullMode = D3D12_CULL_MODE_BACK; break;
    }
}

void RasterizerState::SetFrontClockwise(bool front_is_clockwise) {
    m_desc.FrontCounterClockwise = !front_is_clockwise;
}

const D3D12_RASTERIZER_DESC& RasterizerState::GetState() const {
	return m_desc;
}
