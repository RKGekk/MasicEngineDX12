#pragma once

#include <d3d12.h>

class PrimitiveTopology {
public:
    enum class PrimitiveTopologyEnum { LineList, PointList, TriangleList, TriangleStrip };

    PrimitiveTopology();
    PrimitiveTopology(PrimitiveTopologyEnum topology);
    PrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
    PrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology);

    void SetPrimitiveTopology(PrimitiveTopologyEnum topology);
    void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
    void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology);

    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType() const;
    D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

private:
    PrimitiveTopologyEnum m_topology;
};

