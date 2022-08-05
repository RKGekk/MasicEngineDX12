#include "root_signature.h"

#include "device.h"
#include "../tools/com_exception.h"

#include <cassert>
#include <memory>

RootSignature::RootSignature(Device& device, const D3D12_ROOT_SIGNATURE_DESC1& root_signature_desc) : m_device(device), m_root_signature_desc{}, m_num_descriptors_per_table{ 0 }, m_sampler_table_bit_mask(0), m_descriptor_table_bit_mask(0) {
    SetRootSignatureDesc(root_signature_desc);
}

RootSignature::~RootSignature() {
    Destroy();
}

void RootSignature::Destroy() {
    for (UINT i = 0u; i < m_root_signature_desc.NumParameters; ++i) {
        const D3D12_ROOT_PARAMETER1& root_parameter = m_root_signature_desc.pParameters[i];
        if (root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
            delete[] root_parameter.DescriptorTable.pDescriptorRanges;
        }
    }

    delete[] m_root_signature_desc.pParameters;
    delete[] m_root_signature_desc.pStaticSamplers;
    m_root_signature_desc.pParameters = nullptr;
    m_root_signature_desc.NumParameters = 0u;
    m_root_signature_desc.pStaticSamplers = nullptr;
    m_root_signature_desc.NumStaticSamplers = 0u;

    m_descriptor_table_bit_mask = 0u;
    m_sampler_table_bit_mask = 0u;

    memset(m_num_descriptors_per_table, 0, sizeof(m_num_descriptors_per_table));
}

void RootSignature::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& root_signature_desc) {
    Destroy();

    UINT num_parameters = root_signature_desc.NumParameters;
    D3D12_ROOT_PARAMETER1* pParameters = num_parameters > 0 ? new D3D12_ROOT_PARAMETER1[num_parameters] : nullptr;

    for (UINT i = 0; i < num_parameters; ++i) {
        const D3D12_ROOT_PARAMETER1& root_parameter = root_signature_desc.pParameters[i];
        pParameters[i] = root_parameter;

        if (root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
            UINT num_descriptor_ranges = root_parameter.DescriptorTable.NumDescriptorRanges;
            D3D12_DESCRIPTOR_RANGE1* pDescriptor_ranges = num_descriptor_ranges > 0 ? new D3D12_DESCRIPTOR_RANGE1[num_descriptor_ranges] : nullptr;

            memcpy(pDescriptor_ranges, root_parameter.DescriptorTable.pDescriptorRanges, sizeof(D3D12_DESCRIPTOR_RANGE1) * num_descriptor_ranges);

            pParameters[i].DescriptorTable.NumDescriptorRanges = num_descriptor_ranges;
            pParameters[i].DescriptorTable.pDescriptorRanges = pDescriptor_ranges;

            if (num_descriptor_ranges > 0) {
                switch (pDescriptor_ranges[0].RangeType) {
                    case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                    case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                    case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                        m_descriptor_table_bit_mask |= (1 << i);
                        break;
                    case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
                        m_sampler_table_bit_mask |= (1 << i);
                        break;
                }
            }

            for (UINT j = 0; j < num_descriptor_ranges; ++j) {
                m_num_descriptors_per_table[i] += pDescriptor_ranges[j].NumDescriptors;
            }
        }
    }

    m_root_signature_desc.NumParameters = num_parameters;
    m_root_signature_desc.pParameters = pParameters;

    UINT num_static_samplers = root_signature_desc.NumStaticSamplers;
    D3D12_STATIC_SAMPLER_DESC* pStatic_samplers = num_static_samplers > 0 ? new D3D12_STATIC_SAMPLER_DESC[num_static_samplers] : nullptr;

    if (pStatic_samplers) {
        memcpy(pStatic_samplers, root_signature_desc.pStaticSamplers, sizeof(D3D12_STATIC_SAMPLER_DESC) * num_static_samplers);
    }

    m_root_signature_desc.NumStaticSamplers = num_static_samplers;
    m_root_signature_desc.pStaticSamplers = pStatic_samplers;

    D3D12_ROOT_SIGNATURE_FLAGS flags = root_signature_desc.Flags;
    m_root_signature_desc.Flags = flags;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC version_root_signature_desc;
    version_root_signature_desc.Init_1_1(num_parameters, pParameters, num_static_samplers, pStatic_samplers, flags);

    D3D_ROOT_SIGNATURE_VERSION highest_version = m_device.GetHighestRootSignatureVersion();

    Microsoft::WRL::ComPtr<ID3DBlob> root_signature_blob;
    Microsoft::WRL::ComPtr<ID3DBlob> error_blob;
    HRESULT hr = D3DX12SerializeVersionedRootSignature(&version_root_signature_desc, highest_version, &root_signature_blob, &error_blob);
    ThrowIfFailed(hr);

    auto d3d12_device = m_device.GetD3D12Device();
    hr = d3d12_device->CreateRootSignature(0, root_signature_blob->GetBufferPointer(), root_signature_blob->GetBufferSize(), IID_PPV_ARGS(m_root_signature.ReleaseAndGetAddressOf()));
    ThrowIfFailed(hr);
}

uint32_t RootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_type) const {
    uint32_t descriptor_table_bit_mask = 0u;
    switch (descriptor_heap_type) {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            descriptor_table_bit_mask = m_descriptor_table_bit_mask;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            descriptor_table_bit_mask = m_sampler_table_bit_mask;
            break;
    }

    return descriptor_table_bit_mask;
}

uint32_t RootSignature::GetNumDescriptors(uint32_t root_index) const {
    assert(root_index < 32u);
    return m_num_descriptors_per_table[root_index];
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature::GetD3D12RootSignature() const {
    return m_root_signature;
}

const D3D12_ROOT_SIGNATURE_DESC1& RootSignature::GetRootSignatureDesc() const {
    return m_root_signature_desc;
}
