#include "generate_mips_pso.h"

#include <string>

#include "directx12_wrappers/device.h"
#include "directx12_wrappers/root_signature.h"
#include "directx12_wrappers/pipeline_state_object.h"
#include "../tools/com_exception.h"
#include "../tools/string_utility.h"
#include "directx12_wrappers/shader.h"

#include <directx/d3dx12.h>
#include <d3dcompiler.h>

GenerateMipsPSO::GenerateMipsPSO(Device& device) {
    using namespace std::literals;
    auto d3d12_device = device.GetD3D12Device();

    CD3DX12_DESCRIPTOR_RANGE1 src_mip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
    CD3DX12_DESCRIPTOR_RANGE1 out_mip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 4, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

    CD3DX12_ROOT_PARAMETER1 root_parameters[GenerateMips::NumRootParameters];
    root_parameters[GenerateMips::GenerateMipsCB].InitAsConstants(sizeof(GenerateMipsCB) / 4, 0);
    root_parameters[GenerateMips::SrcMip].InitAsDescriptorTable(1, &src_mip);
    root_parameters[GenerateMips::OutMip].InitAsDescriptorTable(1, &out_mip);

    CD3DX12_STATIC_SAMPLER_DESC linear_clamp_sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc(GenerateMips::NumRootParameters, root_parameters, 1, &linear_clamp_sampler);

    m_root_signature = device.CreateRootSignature("GenerateMipsPSOSignature"s, root_signature_desc);

    static Microsoft::WRL::ComPtr<ID3DBlob> compute_shader_blob;
    static std::wstring compute_shader_name = L"GenerateMips_CS.cso"s;
    if (!compute_shader_blob) {
        HRESULT hr = D3DReadFileToBlob(compute_shader_name.c_str(), compute_shader_blob.GetAddressOf());
        ThrowIfFailed(hr);
    }
    static std::shared_ptr<Shader> compute_shader = std::make_shared<Shader>(compute_shader_blob, "main"s, Shader::Stage::Compute, ConvertString(compute_shader_name));

    m_pipeline_state = device.CreateComputePipelineState("GenerateMipsPSO"s, m_root_signature, compute_shader);
    //m_pipeline_state = std::dynamic_pointer_cast<PipelineStateObject>(device.CreateComputePipelineState("GenerateMipsPSO"s, m_root_signature, compute_shader));

    m_default_uav = device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4u);

    for (UINT i = 0u; i < 4u; ++i) {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uav_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        uav_desc.Texture2D.MipSlice = i;
        uav_desc.Texture2D.PlaneSlice = 0;

        d3d12_device->CreateUnorderedAccessView(nullptr, nullptr, &uav_desc, m_default_uav.GetDescriptorHandle(i));
    }
}

std::shared_ptr<RootSignature> GenerateMipsPSO::GetRootSignature() const {
    return m_root_signature;
}

std::shared_ptr<PipelineStateObject> GenerateMipsPSO::GetPipelineState() const {
    return m_pipeline_state;
}

D3D12_CPU_DESCRIPTOR_HANDLE GenerateMipsPSO::GetDefaultUAV() const {
    return m_default_uav.GetDescriptorHandle();
}