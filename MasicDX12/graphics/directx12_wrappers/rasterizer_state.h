#pragma once

#include <d3d12.h>

class RasterizerState {
public:
    enum class FillMode { Wireframe, Solid };
    enum class CullMode { None, Back, Front };

    RasterizerState();
    RasterizerState(const D3D12_RASTERIZER_DESC& desc);

    void SetFillMode(FillMode mode);
    void SetCullMode(CullMode mode);
    void SetFrontClockwise(bool front_is_clockwise);

    const D3D12_RASTERIZER_DESC& GetState() const;

private:
    D3D12_RASTERIZER_DESC m_desc{};
};