#include "pano_to_cubemap_pso.h"

#include <string>

#include "directx12_wrappers/device.h"
#include "directx12_wrappers/root_signature.h"
#include "directx12_wrappers/pipeline_state_object.h"
#include "../tools/com_exception.h"

#include <directx/d3dx12.h>
#include <d3dcompiler.h>

PanoToCubemapPSO::PanoToCubemapPSO(Device& device) {
    using namespace std::literals;
    auto d3d12_device = device.GetD3D12Device();

    CD3DX12_DESCRIPTOR_RANGE1 src_mip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
    CD3DX12_DESCRIPTOR_RANGE1 out_mip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 5, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

    CD3DX12_ROOT_PARAMETER1 root_parameters[PanoToCubemapRS::NumRootParameters];
    root_parameters[PanoToCubemapRS::PanoToCubemapCB].InitAsConstants(sizeof(PanoToCubemapCB) / 4, 0);
    root_parameters[PanoToCubemapRS::SrcTexture].InitAsDescriptorTable(1, &src_mip);
    root_parameters[PanoToCubemapRS::DstMips].InitAsDescriptorTable(1, &out_mip);

    CD3DX12_STATIC_SAMPLER_DESC linear_repeat_sampler(
        0,
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP
    );

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc(PanoToCubemapRS::NumRootParameters, root_parameters, 1, &linear_repeat_sampler);

    m_root_signature = device.CreateRootSignature("PanoToCubemapPSORootSignature"s, root_signature_desc);

    static Microsoft::WRL::ComPtr<ID3DBlob> compute_shader_blob;
    if (!compute_shader_blob) {
        HRESULT hr = D3DReadFileToBlob(L"PanoToCubemap_CS.cso", compute_shader_blob.GetAddressOf());
        ThrowIfFailed(hr);
    }
    static std::shared_ptr<Shader> compute_shader = std::make_shared<Shader>(compute_shader_blob, "main"s, Shader::Stage::Compute, "PanoToCubemap_CS.cso");

    m_pipeline_state = device.CreateComputePipelineState("PanoToCubemapPSO"s, m_root_signature, compute_shader);

    m_default_uav = device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 5);
    UINT descriptor_handle_increment_size = device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    for (UINT i = 0u; i < 5u; ++i) {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
        uav_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        uav_desc.Texture2DArray.ArraySize = 6; // Cubemap.
        uav_desc.Texture2DArray.FirstArraySlice = 0;
        uav_desc.Texture2DArray.MipSlice = i;
        uav_desc.Texture2DArray.PlaneSlice = 0;

        d3d12_device->CreateUnorderedAccessView(nullptr, nullptr, &uav_desc, m_default_uav.GetDescriptorHandle(i));
    }
}

std::shared_ptr<RootSignature> PanoToCubemapPSO::GetRootSignature() const {
    return m_root_signature;
}

std::shared_ptr<PipelineStateObject> PanoToCubemapPSO::GetPipelineState() const {
    return m_pipeline_state;
}

D3D12_CPU_DESCRIPTOR_HANDLE PanoToCubemapPSO::GetDefaultUAV() const {
    return m_default_uav.GetDescriptorHandle();
}
