#include "root_signature.h"

#include "device.h"
#include "sampler.h"
#include "../tools/com_exception.h"

#include <cassert>
#include <memory>
#include <utility>

RootSignature::RootSignature(Device& device, const std::string& name) : m_device(device), m_root_signature_desc{}, m_num_descriptors_per_table{ 0 }, m_sampler_table_bit_mask(0), m_descriptor_table_bit_mask(0), m_compiled(false), m_name(name) {}

RootSignature::RootSignature(Device& device, const std::string& name, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& root_signature_desc) : m_device(device), m_root_signature_desc{}, m_num_descriptors_per_table{ 0 }, m_sampler_table_bit_mask(0), m_descriptor_table_bit_mask(0), m_compiled(false), m_name(name) {
    SetRootSignatureDesc(root_signature_desc);
}

RootSignature::RootSignature(const RootSignature& other) : m_device(other.m_device), m_root_signature_desc{}, m_num_descriptors_per_table{ 0 }, m_sampler_table_bit_mask(0), m_descriptor_table_bit_mask(0), m_compiled(false) {
    SetRootSignatureDesc(other.m_root_signature_desc);
}

RootSignature::~RootSignature() {
    Destroy();
}

RootSignature& RootSignature::operator=(const RootSignature& right) {
    if (this == &right) {
        return *this;
    }
    SetRootSignatureDesc(right.m_root_signature_desc);
    return *this;
}

void RootSignature::Destroy() {
    m_root_signature_desc.Desc_1_1.pParameters = nullptr;
    m_root_signature_desc.Desc_1_1.NumParameters = 0u;
    m_root_signature_desc.Desc_1_1.pStaticSamplers = nullptr;
    m_root_signature_desc.Desc_1_1.NumStaticSamplers = 0u;

    m_descriptor_table_bit_mask = 0u;
    m_sampler_table_bit_mask = 0u;
    m_bytes_used = 0u;

    m_parameters.clear();
    m_static_samplers.clear();
    m_parameter_indices_map.clear();

    memset(m_num_descriptors_per_table, 0, sizeof(m_num_descriptors_per_table));
}

D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignature::CombineRootSignatureDesc(D3D12_ROOT_SIGNATURE_FLAGS flags) {
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC res{};

    res.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    res.Desc_1_1.Flags = flags;

    res.Desc_1_1.NumParameters = (UINT)m_parameters.size();
    res.Desc_1_1.pParameters = m_parameters.data();

    res.Desc_1_1.NumStaticSamplers = (UINT)m_static_samplers.size();
    res.Desc_1_1.pStaticSamplers = m_static_samplers.data();

    return res;
}

void RootSignature::CompileRootSignature() {
    //D3D_ROOT_SIGNATURE_VERSION highest_version = m_device.GetHighestRootSignatureVersion();
    D3D_ROOT_SIGNATURE_VERSION highest_version = D3D_ROOT_SIGNATURE_VERSION_1_1;

    Microsoft::WRL::ComPtr<ID3DBlob> root_signature_blob;
    Microsoft::WRL::ComPtr<ID3DBlob> error_blob;
    HRESULT hr = D3DX12SerializeVersionedRootSignature(&m_root_signature_desc, highest_version, &root_signature_blob, &error_blob);
    ThrowIfFailed(hr);

    auto d3d12_device = m_device.GetD3D12Device();
    hr = d3d12_device->CreateRootSignature(0, root_signature_blob->GetBufferPointer(), root_signature_blob->GetBufferSize(), IID_PPV_ARGS(m_root_signature.ReleaseAndGetAddressOf()));
    ThrowIfFailed(hr);

    //m_root_signature->SetPrivateData(RootSignature::GetGUID(), sizeof(RootSignature*), this);

    m_compiled = true;
}

void RootSignature::SetRootSignatureDesc(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& root_signature_desc) {
    Destroy();

    UINT num_parameters = root_signature_desc.Desc_1_1.NumParameters;
    for (UINT i = 0; i < num_parameters; ++i) {
        const D3D12_ROOT_PARAMETER1& root_parameter = root_signature_desc.Desc_1_1.pParameters[i];
        if (root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
            m_bytes_used += 4u;

            UINT num_descriptor_ranges = root_parameter.DescriptorTable.NumDescriptorRanges;
            if (num_descriptor_ranges > 0) {
                switch (root_parameter.DescriptorTable.pDescriptorRanges[0].RangeType) {
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
            RootDescriptorTableParameter table_param = RootDescriptorTableParameter(root_parameter.DescriptorTable, root_parameter.ShaderVisibility);
            for (const SignatureRegisters& location : table_param.SignatureLocations()) {
                assert(m_parameter_indices_map.find(location) == m_parameter_indices_map.end()); // Register of such slot, space and type is already occupied in this root signature
                m_parameter_indices_map[location] = i;
            }
            m_descriptor_table_parameters.push_back(std::move(table_param));
            for (UINT j = 0; j < num_descriptor_ranges; ++j) {
                m_num_descriptors_per_table[i] += root_parameter.DescriptorTable.pDescriptorRanges[j].NumDescriptors;
            }
        }
        if (root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV || root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV || root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV) {
            RootDescriptorParameter desc_param = RootDescriptorParameter(root_parameter.ParameterType, root_parameter.Descriptor, root_parameter.ShaderVisibility);
            for (const SignatureRegisters& location : desc_param.SignatureLocations()) {
                assert(m_parameter_indices_map.find(location) == m_parameter_indices_map.end()); // Register of such slot, space and type is already occupied in this root signature
                m_parameter_indices_map[location] = i;
            }
            m_descriptor_parameters.push_back(std::move(desc_param));
            m_bytes_used += 8u;
        }
        if (root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS) {
            m_bytes_used += 4u * root_parameter.Constants.Num32BitValues;
            RootConstantsParameter const_param = RootConstantsParameter(root_parameter.Constants, root_parameter.ShaderVisibility);
            for (const SignatureRegisters& location : const_param.SignatureLocations()) {
                assert(m_parameter_indices_map.find(location) == m_parameter_indices_map.end()); // Register of such slot, space and type is already occupied in this root signature
                m_parameter_indices_map[location] = i;
            }
            m_constant_parameters.push_back(std::move(const_param));
        }
        m_parameters.push_back(root_parameter);
    }

    UINT num_samplers = root_signature_desc.Desc_1_1.NumStaticSamplers;
    for (UINT i = 0; i < num_samplers; ++i) {
        m_static_samplers.push_back(root_signature_desc.Desc_1_1.pStaticSamplers[i]);
    }

    m_root_signature_desc = CombineRootSignatureDesc(root_signature_desc.Desc_1_1.Flags);

    CompileRootSignature();
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

uint32_t RootSignature::GetBytesUsed() const {
    return m_bytes_used;
}

const std::string& RootSignature::GetName() const {
    return m_name;
}

bool RootSignature::ConatinParameterIndex(const SignatureRegisters& location) const {
    return m_parameter_indices_map.count(location);
}

RootSignature::ParameterIndex RootSignature::GetParameterIndex(const SignatureRegisters& location) const {
    return m_parameter_indices_map.find(location)->second;
}

uint16_t RootSignature::ParameterCount() const {
    return (uint16_t)(m_descriptor_table_parameters.size() + m_descriptor_parameters.size() + m_constant_parameters.size());
}

void RootSignature::AddDescriptorTableParameter(const RootDescriptorTableParameter& table) {
    m_compiled = false;
    ParameterIndex index = (ParameterIndex)m_parameters.size();
    for (const SignatureRegisters& location : table.SignatureLocations()) {
        assert(m_parameter_indices_map.find(location) != m_parameter_indices_map.end()); // Register of such slot, space and type is already occupied in this root signature
        m_parameter_indices_map[location] = index;
    }
    m_descriptor_table_parameters.push_back(table);

    const D3D12_ROOT_PARAMETER1& root_parameter = table.GetParameter();
    m_parameters.push_back(root_parameter);
    m_descriptor_table_bit_mask |= (1 << index);
    UINT num_descriptor_ranges = root_parameter.DescriptorTable.NumDescriptorRanges;
    for (UINT j = 0; j < num_descriptor_ranges; ++j) {
        m_num_descriptors_per_table[index] += root_parameter.DescriptorTable.pDescriptorRanges[j].NumDescriptors;
    }
}

void RootSignature::AddDescriptorParameter(const RootDescriptorParameter& descriptor) {
    m_compiled = false;
    ParameterIndex index = (ParameterIndex)m_parameters.size();
    for (const SignatureRegisters& location : descriptor.SignatureLocations()) {
        assert(m_parameter_indices_map.find(location) != m_parameter_indices_map.end()); // Register of such slot, space and type is already occupied in this root signature
        m_parameter_indices_map[location] = index;
    }
    m_descriptor_parameters.push_back(descriptor);
    m_parameters.push_back(descriptor.GetParameter());
}

void RootSignature::AddConstantsParameter(const RootConstantsParameter& constants) {
    m_compiled = false;
    ParameterIndex index = (ParameterIndex)m_parameters.size();
    for (const SignatureRegisters& location : constants.SignatureLocations()) {
        assert(m_parameter_indices_map.find(location) != m_parameter_indices_map.end()); // Register of such slot, space and type is already occupied in this root signature
        m_parameter_indices_map[location] = index;
    }
    m_constant_parameters.push_back(constants);
    m_parameters.push_back(constants.GetParameter());
}

void RootSignature::AddStaticSampler(const RootSaticSampler& sampler) {
    m_compiled = false;
    ParameterIndex index = (ParameterIndex)m_parameters.size();

    const SignatureRegisters& location = sampler.GetSignatureLocation();
    assert(m_parameter_indices_map.find(location) != m_parameter_indices_map.end()); // Register of such slot, space and type is already occupied in this root signature
    m_parameter_indices_map[location] = index;

    m_root_static_samplers.push_back(sampler);
    m_static_samplers.push_back(sampler.GetStaticSampler());
}

void RootSignature::SetRootSignatureDescFlags(D3D12_ROOT_SIGNATURE_FLAGS flags) {
    m_root_signature_desc.Desc_1_1.Flags = flags;
}

//GUID RootSignature::GetGUID() {
//    static UUID uuid;
//    static bool generated = false;
//    if (!generated) {
//        UuidCreate(&uuid);
//        generated = true;
//    }
//    return uuid;
//}

Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature::GetD3D12RootSignature() {
    if(!m_compiled) {
        m_root_signature_desc = CombineRootSignatureDesc(m_root_signature_desc.Desc_1_1.Flags);
        CompileRootSignature();
    }
    return m_root_signature;
}

const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& RootSignature::GetRootSignatureDesc() {
    if (!m_compiled) {
        m_root_signature_desc = CombineRootSignatureDesc(m_root_signature_desc.Desc_1_1.Flags);
        CompileRootSignature();
    }
    return m_root_signature_desc;
}
