#pragma once

#include <d3d12.h>

class DepthStencilState {
public:
    enum class ComparisonFunction { Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always };

    DepthStencilState();
    DepthStencilState(D3D12_DEPTH_STENCIL_DESC desc);

    void SetDepthTestEnabled(bool enabled);
    void SetDepthWriteEnabled(bool enabled);
    void SetComparisonFunction(ComparisonFunction function);

    const D3D12_DEPTH_STENCIL_DESC& GetState() const;

private:
    D3D12_DEPTH_STENCIL_DESC m_desc{};
};