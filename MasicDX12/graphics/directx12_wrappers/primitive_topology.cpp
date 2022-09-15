#include "primitive_topology.h"

#include <cassert>

PrimitiveTopology::PrimitiveTopology() : m_topology(PrimitiveTopologyEnum::TriangleList) {}

PrimitiveTopology::PrimitiveTopology(PrimitiveTopologyEnum topology) : m_topology(topology) {}

PrimitiveTopology::PrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) {
    SetPrimitiveTopology(topology);
}

PrimitiveTopology::PrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology) {
    SetPrimitiveTopology(topology);
}

void PrimitiveTopology::SetPrimitiveTopology(PrimitiveTopologyEnum topology) {
    m_topology = topology;
}

void PrimitiveTopology::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) {
    switch (topology) {
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED: assert(false); break; //Should never be hit 
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST: m_topology = PrimitiveTopologyEnum::PointList; break;
        case D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST: m_topology = PrimitiveTopologyEnum::LineList; break;
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST: m_topology = PrimitiveTopologyEnum::TriangleList; break;
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: m_topology = PrimitiveTopologyEnum::TriangleStrip; break;
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        case D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST: assert(false); break; //Should never be hit
        default:
            break;
    }
}

void PrimitiveTopology::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology) {
    switch (topology) {
        case D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED: assert(false, "Should never be hit"); break;
        case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT: m_topology = PrimitiveTopologyEnum::PointList; break;
        case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE: m_topology = PrimitiveTopologyEnum::LineList; break;
        case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE: m_topology = PrimitiveTopologyEnum::TriangleList; break;
        case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH: assert(false, "Should never be hit"); break;
        default: assert(false, "Should never be hit"); break;
    }
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopology::GetPrimitiveTopologyType() const {
    switch (m_topology) {
        case PrimitiveTopologyEnum::LineList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case PrimitiveTopologyEnum::PointList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case PrimitiveTopologyEnum::TriangleStrip:
        case PrimitiveTopologyEnum::TriangleList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default: assert(false, "Should never be hit"); return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    }
}

D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology::GetPrimitiveTopology() const {
    switch (m_topology) {
        case PrimitiveTopologyEnum::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case PrimitiveTopologyEnum::PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case PrimitiveTopologyEnum::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case PrimitiveTopologyEnum::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default: assert(false, "Should never be hit"); return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
}
