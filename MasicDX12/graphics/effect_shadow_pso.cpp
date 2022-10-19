#include "effect_shadow_pso.h"

#include "directx12_wrappers/command_list.h"
#include "directx12_wrappers/device.h"
#include "directx12_wrappers/pipeline_state_object.h"
#include "directx12_wrappers/root_signature.h"
#include "vertex_types.h"
#include "../tools/com_exception.h"
#include "../tools/string_utility.h"
#include "../tools/memory_utility.h"
#include "../nodes/shadow_manager.h"
#include "../nodes/shadow_camera_node.h"

#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include <wrl/client.h>

EffectShadowPSO::EffectShadowPSO(std::shared_ptr<Device> device, std::shared_ptr<ShadowManager> shadow_manager) : m_device(device), m_dirty_flags(DF_All), m_pPrevious_command_list(nullptr), m_need_transpose(true), m_shadow_manager(shadow_manager) {
    using namespace std::literals;
    m_pAligned_mvp = (MVP*)_aligned_malloc(sizeof(MVP), 16);

    Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader_blob;
    HRESULT hr = D3DReadFileToBlob(L"BasicShadow_VS.cso", vertex_shader_blob.GetAddressOf());
    ThrowIfFailed(hr);
    m_vertex_shader = std::make_shared<VertexShader>(vertex_shader_blob, "main"s, "BasicShadow_VS.cso"s);

    Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader_blob;
    m_pixel_shader = std::make_shared<PixelShader>(pixel_shader_blob, "main"s, "Empty", true);

    D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_ROOT_PARAMETER1 root_parameters[RootParameters::NumRootParameters];
    root_parameters[RootParameters::MatricesCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description;
    root_signature_description.Init_1_1(RootParameters::NumRootParameters, root_parameters, 0u, nullptr, root_signature_flags);

    m_root_signature = m_device->CreateRootSignature("RootSignForBasicShadow"s, root_signature_description);

    DXGI_FORMAT depth_buffer_format = DXGI_FORMAT_D32_FLOAT;

    m_vertex_shader->AddRegister({ 0, 0, ShaderRegister::ConstantBuffer }, "MatricesCB"s);
    m_vertex_shader->SetInputAssemblerLayout(VertexPositionNormalTangentBitangentTexture::InputLayout);
    m_vertex_shader->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

    std::shared_ptr<ShadowCameraNode> shadow_node = m_shadow_manager->GetShadow();
    ShadowCameraNode::ShadowCameraProps shadow_props = shadow_node->GetShadowProps();
    CD3DX12_RASTERIZER_DESC rasterizer_state(D3D12_DEFAULT);
    rasterizer_state.DepthBias = shadow_props.DepthBias;
    rasterizer_state.DepthBiasClamp = shadow_props.DepthBiasClamp;
    rasterizer_state.SlopeScaledDepthBias = shadow_props.SlopeScaledDepthBias;
    m_pixel_shader->SetRenderTargetFormat(AttachmentPoint::DepthStencil, m_shadow_manager->GetShadowBufferFormat());
    m_pixel_shader->SetRasterizerState(rasterizer_state);

    m_pipeline_state_object = m_device->CreateGraphicsPipelineState("PSOForBasicShadow"s, m_root_signature, m_vertex_shader, m_pixel_shader);

    D3D12_SHADER_RESOURCE_VIEW_DESC default_srv;
    default_srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    default_srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    default_srv.Texture2D.MostDetailedMip = 0;
    default_srv.Texture2D.MipLevels = 1;
    default_srv.Texture2D.PlaneSlice = 0;
    default_srv.Texture2D.ResourceMinLODClamp = 0;
    default_srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    m_default_srv = m_device->CreateShaderResourceView(nullptr, &default_srv);
}

EffectShadowPSO::~EffectShadowPSO() {
    _aligned_free(m_pAligned_mvp);
}

void EffectShadowPSO::Apply(CommandList& command_list) {
    command_list.SetPipelineState(m_pipeline_state_object);
    command_list.SetGraphicsRootSignature(m_root_signature);

    if (m_dirty_flags & DF_Matrices) {
        Matrices m;
        m.ModelMatrix = m_pAligned_mvp->World;
        m.ModelViewMatrix = m_pAligned_mvp->World * m_pAligned_mvp->View;
        m.ModelViewProjectionMatrix = m.ModelViewMatrix * m_pAligned_mvp->Projection;
        m.InverseTransposeModelViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m.ModelViewMatrix));

        command_list.SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, m);
    }

    m_dirty_flags = DF_None;
}

void XM_CALLCONV EffectShadowPSO::SetWorldMatrix(DirectX::FXMMATRIX world_matrix) {
    m_pAligned_mvp->World = world_matrix;
    m_dirty_flags |= DF_Matrices;
}

void XM_CALLCONV EffectShadowPSO::SetViewMatrix(DirectX::FXMMATRIX view_matrix) {
    m_pAligned_mvp->View = view_matrix;
    m_dirty_flags |= DF_Matrices;
}

void XM_CALLCONV EffectShadowPSO::SetProjectionMatrix(DirectX::FXMMATRIX projection_matrix) {
    m_pAligned_mvp->Projection = projection_matrix;
    m_dirty_flags |= DF_Matrices;
}