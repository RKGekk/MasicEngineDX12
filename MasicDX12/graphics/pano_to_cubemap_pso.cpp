#include "pano_to_cubemap_pso.h"

#include "device.h"
#include "root_signature.h"
#include "../tools/com_exception.h"

#include <d3dx12.h>
#include <d3dcompiler.h>

PanoToCubemapPSO::PanoToCubemapPSO(Device& device) {
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

    m_root_signature = device.CreateRootSignature(root_signature_desc.Desc_1_1);

    struct PipelineStateStream {
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRoot_signature;
        CD3DX12_PIPELINE_STATE_STREAM_CS CS;
    } pipeline_state_stream;

    static Microsoft::WRL::ComPtr<ID3DBlob> compute_shader_blob;
    if (!compute_shader_blob) {
        HRESULT hr = D3DReadFileToBlob(L"PanoToCubemap_CS.cso", compute_shader_blob.GetAddressOf());
        ThrowIfFailed(hr);
    }

    pipeline_state_stream.pRoot_signature = m_root_signature->GetD3D12RootSignature().Get();
    pipeline_state_stream.CS = CD3DX12_SHADER_BYTECODE(compute_shader_blob.Get());

    m_pipeline_state = device.CreatePipelineStateObject(pipeline_state_stream);

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
