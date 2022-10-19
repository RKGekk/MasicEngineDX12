#include "effect_shadow_instanced_pso.h"

#include "d3d12_renderer.h"
#include "directx12_wrappers/command_list.h"
#include "directx12_wrappers/device.h"
#include "directx12_wrappers/pipeline_state_object.h"
#include "directx12_wrappers/root_signature.h"
#include "directx12_wrappers/structured_buffer.h"
#include "directx12_wrappers/swap_chain.h"
#include "vertex_types.h"
#include "../engine/engine.h"
#include "../tools/com_exception.h"
#include "../tools/memory_utility.h"
#include "../tools/string_utility.h"
#include "../nodes/camera_node.h"
#include "../nodes/shadow_camera_node.h"
#include "../nodes/mesh_manager.h"
#include "../nodes/shadow_manager.h"
#include "../nodes/mesh_node.h"

#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include <wrl/client.h>

EffectShadowInstancedPSO::EffectShadowInstancedPSO(std::shared_ptr<Device> device, std::shared_ptr<ShadowManager> shadow_manager) : m_device(device), m_dirty_flags(DF_All), m_pPrevious_command_list(nullptr), m_need_transpose(false), m_shadow_manager(shadow_manager) {
    using namespace std::literals;
    m_pAligned_mvp = (VP*)_aligned_malloc(sizeof(VP), 16);

    Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader_blob;
    std::string vertex_shader_name = "BaseInstanced_VS.cso";
    HRESULT hr = D3DReadFileToBlob(to_wstring(vertex_shader_name).c_str(), vertex_shader_blob.GetAddressOf());
    ThrowIfFailed(hr);
    m_vertex_shader = std::make_shared<VertexShader>(vertex_shader_blob, "main"s, vertex_shader_name);

    Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader_blob;
    m_pixel_shader = std::make_shared<PixelShader>(pixel_shader_blob, "main"s, "Empty", true);

    D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
    CD3DX12_DESCRIPTOR_RANGE1 descriptor_rage_instance(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1);

    CD3DX12_ROOT_PARAMETER1 root_parameters[to_underlying(RootParameters::NumRootParameters)];

    root_parameters[to_underlying(RootParameters::PerPassData)].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
    root_parameters[to_underlying(RootParameters::InstanceData)].InitAsDescriptorTable(1, &descriptor_rage_instance, D3D12_SHADER_VISIBILITY_VERTEX);
    root_parameters[to_underlying(RootParameters::InstanceIndexData)].InitAsShaderResourceView(1, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description;
    root_signature_description.Init_1_1(to_underlying(RootParameters::NumRootParameters), root_parameters, 0u, nullptr, root_signature_flags);

    m_root_signature = m_device->CreateRootSignature("RootSignFor"s + vertex_shader_name, root_signature_description);

    std::shared_ptr<Engine> engine = Engine::GetEngine();
    std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());

    m_vertex_shader->AddRegister({ 0, 0, ShaderRegister::ConstantBuffer }, "gPerPassData"s);
    m_vertex_shader->AddRegister({ 0, 1, ShaderRegister::ShaderResource }, "gInstanceData"s);
    m_vertex_shader->AddRegister({ 1, 1, ShaderRegister::ShaderResource }, "gInstanceIndexData"s);
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

    m_pipeline_state_object = m_device->CreateGraphicsPipelineState("PSOFor"s + vertex_shader_name, m_root_signature, m_vertex_shader, m_pixel_shader);

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

EffectShadowInstancedPSO::~EffectShadowInstancedPSO() {
    _aligned_free(m_pAligned_mvp);
}

void EffectShadowInstancedPSO::SetMeshManager(std::shared_ptr<MeshManager> mesh_manager) {
    m_mesh_manager = mesh_manager;
    m_dirty_flags |= DF_InstanceData | DF_PerPassData;
}

void EffectShadowInstancedPSO::Apply(CommandList& command_list, const GameTimerDelta& delta) {
    command_list.SetPipelineState(m_pipeline_state_object);
    command_list.SetGraphicsRootSignature(m_root_signature);

    if (m_dirty_flags & DF_PerPassData) {
        PerPassData per_pass_data;
        per_pass_data.ViewMatrix = m_pAligned_mvp->View;
        per_pass_data.InverseTransposeViewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, per_pass_data.ViewMatrix));

        per_pass_data.ProjectionMatrix = m_pAligned_mvp->Projection;
        per_pass_data.InverseTransposeProjectionMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, per_pass_data.ProjectionMatrix));

        per_pass_data.ViewProjectionMatrix = per_pass_data.ViewMatrix * m_pAligned_mvp->Projection;
        per_pass_data.InverseTransposeViewProjectionMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, per_pass_data.ViewProjectionMatrix));

        per_pass_data.RenderTargetSizeX = m_render_target_size.x;
        per_pass_data.RenderTargetSizeY = m_render_target_size.y;
        per_pass_data.InverseRenderTargetSizeX = m_render_target_size.z;
        per_pass_data.InverseRenderTargetSizeY = m_render_target_size.w;

        per_pass_data.NearZ = m_near_z;
        per_pass_data.FarZ = m_far_z;

        per_pass_data.TotalTime = delta.GetTotalSeconds();
        per_pass_data.DeltaTime = delta.GetDeltaSeconds();

        command_list.SetGraphicsDynamicConstantBuffer(to_underlying(RootParameters::PerPassData), per_pass_data);
    }

    const MeshManager::MeshMap& mesh_map = m_mesh_manager->GetMeshMap();
    for (const auto& [mesh_name, mesh_list] : mesh_map) {
        auto instance_buffer_view = m_mesh_manager->GetInstanceBufferView(mesh_name);
        command_list.SetShaderResourceView(to_underlying(RootParameters::InstanceData), 0, instance_buffer_view);

        const auto& instance_index_list = m_mesh_manager->GetInstanceIndexData(mesh_name);
        command_list.SetGraphicsDynamicStructuredBuffer(to_underlying(RootParameters::InstanceIndexData), instance_index_list);

        std::shared_ptr<MeshNode> mesh_node = mesh_list[0u];
        const MeshNode::MeshList& mesh_list = mesh_node->GetMeshes();
        for (const auto& mesh : mesh_list) {
            command_list.SetPrimitiveTopology(mesh->GetPrimitiveTopology());

            const Mesh::BufferMap& buffer_map = mesh->GetVertexBuffers();
            for (auto vertex_buffer : buffer_map) {
                command_list.SetVertexBuffer(vertex_buffer.first, vertex_buffer.second);
            }

            size_t index_count = mesh->GetIndexCount();
            size_t vertex_count = mesh->GetVertexCount();
            size_t instance_count = instance_index_list.size();
            size_t start_instance = 0u;
            if (index_count > 0u) {
                command_list.SetIndexBuffer(mesh->GetIndexBuffer());
                command_list.DrawIndexed(index_count, instance_count, 0u, 0u, start_instance);
            }
            else if (vertex_count > 0u) {
                command_list.Draw(vertex_count, instance_count, 0u, start_instance);
            }
        }
    }

    m_dirty_flags = DF_None;
}

void EffectShadowInstancedPSO::SetViewMatrix(const ShadowCameraNode& camera) {
    m_pAligned_mvp->View = camera.GetView();
    m_pAligned_mvp->Projection = camera.GetProjection();
    m_near_z = 0.0f;
    m_far_z = camera.GetFrustum().Extents.z;
    m_dirty_flags |= DF_PerPassData | DF_Near_Far | DF_Near_Far;
}

void XM_CALLCONV EffectShadowInstancedPSO::SetViewMatrix(DirectX::FXMMATRIX view_matrix) {
    m_pAligned_mvp->View = view_matrix;
    m_dirty_flags |= DF_PerPassData;
}

void XM_CALLCONV EffectShadowInstancedPSO::SetProjectionMatrix(DirectX::FXMMATRIX projection_matrix) {
    m_pAligned_mvp->Projection = projection_matrix;
    m_dirty_flags |= DF_PerPassData;
}

void EffectShadowInstancedPSO::SetNearZ(float near_z) {
    m_near_z = near_z;
    m_dirty_flags |= DF_Near_Far;
}

void EffectShadowInstancedPSO::SetFarZ(float far_z) {
    m_far_z = far_z;
    m_dirty_flags |= DF_Near_Far;
}

void EffectShadowInstancedPSO::SetRenderTargetSize(DirectX::XMFLOAT2 render_target_size) {
    m_render_target_size.x = render_target_size.x;
    m_render_target_size.y = render_target_size.y;
    m_render_target_size.z = 1.0f / render_target_size.x;
    m_render_target_size.w = 1.0f / render_target_size.y;
    m_dirty_flags |= DF_RT_Size;
}
