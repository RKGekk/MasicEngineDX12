#include "effect_instanced_pso.h"

#include "d3d12_renderer.h"
#include "directx12_wrappers/command_list.h"
#include "directx12_wrappers/device.h"
#include "directx12_wrappers/pipeline_state_object.h"
#include "directx12_wrappers/root_signature.h"
#include "directx12_wrappers/structured_buffer.h"
#include "directx12_wrappers/swap_chain.h"
#include "material.h"
#include "vertex_types.h"
#include "../engine/engine.h"
#include "../tools/com_exception.h"
#include "../tools/memory_utility.h"
#include "../tools/string_utility.h"
#include "../nodes/scene.h"
#include "../nodes/camera_node.h"
#include "../nodes/basic_camera_node.h"
#include "../nodes/light_manager.h"
#include "../nodes/shadow_manager.h"
#include "../nodes/shadow_camera_node.h"
#include "../nodes/mesh_manager.h"
#include "../nodes/mesh_node.h"

#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include <wrl/client.h>

EffectInstancedPSO::EffectInstancedPSO(std::shared_ptr<Device> device) : m_device(device), m_dirty_flags(DF_All), m_pPrevious_command_list(nullptr), m_need_transpose(false) {
    using namespace std::literals;
    m_pAligned_mvp = (VP*)_aligned_malloc(sizeof(VP), 16);

    Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader_blob;
    std::string vertex_shader_name = "BaseInstanced_VS.cso";
    HRESULT hr = D3DReadFileToBlob(to_wstring(vertex_shader_name).c_str(), vertex_shader_blob.GetAddressOf());
    ThrowIfFailed(hr);
    m_vertex_shader = std::make_shared<VertexShader>(vertex_shader_blob, "main"s, vertex_shader_name);

    Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader_blob;
    std::string pixel_shader_name = "BaseInstanced_PS.cso"s;
    hr = D3DReadFileToBlob(to_wstring(pixel_shader_name).c_str(), pixel_shader_blob.GetAddressOf());
    ThrowIfFailed(hr);
    m_pixel_shader = std::make_shared<PixelShader>(pixel_shader_blob, "main"s, pixel_shader_name);

    D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
    CD3DX12_DESCRIPTOR_RANGE1 descriptor_rage_texture(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, to_underlying(Material::TextureType::NumTypes), 3);
    CD3DX12_DESCRIPTOR_RANGE1 descriptor_rage_instance(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1);

    CD3DX12_ROOT_PARAMETER1 root_parameters[to_underlying(RootParameters::NumRootParameters)];

    root_parameters[to_underlying(RootParameters::PerPassData)].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
    //root_parameters[to_underlying(RootParameters::InstanceData)].InitAsShaderResourceView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    root_parameters[to_underlying(RootParameters::InstanceData)].InitAsDescriptorTable(1, &descriptor_rage_instance, D3D12_SHADER_VISIBILITY_VERTEX);
    root_parameters[to_underlying(RootParameters::InstanceIndexData)].InitAsShaderResourceView(1, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    root_parameters[to_underlying(RootParameters::MaterialCB)].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[to_underlying(RootParameters::LightPropertiesCB)].InitAsConstants(sizeof(LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[to_underlying(RootParameters::FogPropertiesCB)].InitAsConstants(sizeof(FogProperties) / 4, 2, 0, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[to_underlying(RootParameters::PointLights)].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[to_underlying(RootParameters::SpotLights)].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[to_underlying(RootParameters::DirectionalLights)].InitAsShaderResourceView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[to_underlying(RootParameters::Textures)].InitAsDescriptorTable(1, &descriptor_rage_texture, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_STATIC_SAMPLER_DESC anisotropic_sampler(0, D3D12_FILTER_ANISOTROPIC);
    const CD3DX12_STATIC_SAMPLER_DESC shadow_sampler(1, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> samplers = { anisotropic_sampler, shadow_sampler };

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description;
    root_signature_description.Init_1_1(to_underlying(RootParameters::NumRootParameters), root_parameters, samplers.size(), samplers.data(), root_signature_flags);

    m_root_signature = m_device->CreateRootSignature("RootSignFor"s + pixel_shader_name, root_signature_description);

    std::shared_ptr<Engine> engine = Engine::GetEngine();
    std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());

    //DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    m_back_buffer_format = renderer->GetBackBufferFormat();
    m_depth_buffer_format = renderer->GetDepthBufferFormat();

    //DXGI_SAMPLE_DESC sample_desc = m_device->GetMultisampleQualityLevels(back_buffer_format);
    m_sample_desc = { 1, 0 };

    D3D12_RT_FORMAT_ARRAY rtv_formats = {};
    rtv_formats.NumRenderTargets = 1;
    rtv_formats.RTFormats[0] = m_back_buffer_format;

    CD3DX12_RASTERIZER_DESC rasterizer_state(D3D12_DEFAULT);

    m_vertex_shader->AddRegister({ 0, 0, ShaderRegister::ConstantBuffer }, "gPerPassData"s);
    m_vertex_shader->AddRegister({ 0, 1, ShaderRegister::ShaderResource }, "gInstanceData"s);
    m_vertex_shader->AddRegister({ 1, 1, ShaderRegister::ShaderResource }, "gInstanceIndexData"s);
    m_vertex_shader->SetInputAssemblerLayout(VertexPositionNormalTangentBitangentTexture::InputLayout);
    m_vertex_shader->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

    m_pixel_shader->AddRegister({ 0, 0, ShaderRegister::ConstantBuffer }, "gPerPassData"s);
    m_pixel_shader->AddRegister({ 0, 1, ShaderRegister::ConstantBuffer }, "MaterialCB"s);
    m_pixel_shader->AddRegister({ 1, 0, ShaderRegister::ConstantBuffer }, "LightPropertiesCB");
    m_pixel_shader->AddRegister({ 2, 0, ShaderRegister::ConstantBuffer }, "FogPropertiesCB");
    m_pixel_shader->AddRegister({ 0, 0, ShaderRegister::ShaderResource }, "PointLights"s);
    m_pixel_shader->AddRegister({ 1, 0, ShaderRegister::ShaderResource }, "SpotLights"s);
    m_pixel_shader->AddRegister({ 2, 0, ShaderRegister::ShaderResource }, "DirectionalLights"s);
    m_pixel_shader->AddRegister({ 3, 0, ShaderRegister::ShaderResource }, "AmbientTexture"s);
    m_pixel_shader->AddRegister({ 4, 0, ShaderRegister::ShaderResource }, "EmissiveTexture"s);
    m_pixel_shader->AddRegister({ 5, 0, ShaderRegister::ShaderResource }, "DiffuseTexture"s);
    m_pixel_shader->AddRegister({ 6, 0, ShaderRegister::ShaderResource }, "SpecularTexture"s);
    m_pixel_shader->AddRegister({ 7, 0, ShaderRegister::ShaderResource }, "SpecularPowerTexture"s);
    m_pixel_shader->AddRegister({ 8, 0, ShaderRegister::ShaderResource }, "NormalTexture"s);
    m_pixel_shader->AddRegister({ 9, 0, ShaderRegister::ShaderResource }, "BumpTexture"s);
    m_pixel_shader->AddRegister({ 10, 0, ShaderRegister::ShaderResource }, "OpacityTexture"s);
    m_pixel_shader->AddRegister({ 11, 0, ShaderRegister::ShaderResource }, "DisplacementTexture"s);
    m_pixel_shader->AddRegister({ 12, 0, ShaderRegister::ShaderResource }, "MetalnessTexture"s);
    m_pixel_shader->AddRegister({ 13, 0, ShaderRegister::ShaderResource }, "ShadowTexture"s);
    m_pixel_shader->AddRegister({ 0, 0, ShaderRegister::Sampler }, "TextureSampler"s);
    m_pixel_shader->AddRegister({ 1, 0, ShaderRegister::Sampler }, "ShadowSampler"s);
    m_pixel_shader->SetRenderTargetFormat(rtv_formats);
    m_pixel_shader->SetRenderTargetFormat(AttachmentPoint::DepthStencil, m_depth_buffer_format);
    //m_pixel_shader->SetBlendState();
    m_pixel_shader->SetRasterizerState(rasterizer_state);
    //m_pixel_shader->SetDepthStencilState();
    m_pixel_shader->SetSample(m_sample_desc);

    m_pipeline_state_object = m_device->CreateGraphicsPipelineState("PSOFor"s + pixel_shader_name, m_root_signature, m_vertex_shader, m_pixel_shader);

    D3D12_SHADER_RESOURCE_VIEW_DESC default_srv;
    default_srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    default_srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    default_srv.Texture2D.MostDetailedMip = 0;
    default_srv.Texture2D.MipLevels = 1;
    default_srv.Texture2D.PlaneSlice = 0;
    default_srv.Texture2D.ResourceMinLODClamp = 0;
    default_srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    m_default_srv = m_device->CreateShaderResourceView(nullptr, &default_srv);

    m_fog_properties.FogColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_fog_properties.FogStart = 1.0f;
    m_fog_properties.FogRange = 10.0f;
}

EffectInstancedPSO::~EffectInstancedPSO() {
    _aligned_free(m_pAligned_mvp);
}

void EffectInstancedPSO::SetLightManager(std::shared_ptr<LightManager> light_manager) {
    m_light_manager = light_manager;
    m_dirty_flags |= DF_PointLights | DF_SpotLights | DF_DirectionalLights;
}

void EffectInstancedPSO::SetShadowManager(std::shared_ptr<ShadowManager> shadow_manager) {
    m_shadow_manager = shadow_manager;
    m_dirty_flags |= DF_InstanceData | DF_PerPassData;
}

void EffectInstancedPSO::SetMeshManager(std::shared_ptr<MeshManager> mesh_manager) {
    m_mesh_manager = mesh_manager;
    m_dirty_flags |= DF_InstanceData | DF_PerPassData;
}

inline void EffectInstancedPSO::BindTexture(CommandList& command_list, uint32_t offset, const std::shared_ptr<Texture>& texture) {
    if (texture) command_list.SetShaderResourceView(to_underlying(RootParameters::Textures), offset, texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    else command_list.SetShaderResourceView(to_underlying(RootParameters::Textures), offset, m_default_srv, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void EffectInstancedPSO::Apply(CommandList& command_list, const GameTimerDelta& delta) {
    command_list.SetPipelineState(m_pipeline_state_object);
    command_list.SetGraphicsRootSignature(m_root_signature);

    std::shared_ptr<ShadowCameraNode> shadow_camera = nullptr;
    if (m_shadow_manager) {
        shadow_camera = m_shadow_manager->GetShadow();
    }

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

        if (shadow_camera) per_pass_data.ShadowTransform = DirectX::XMMatrixTranspose(shadow_camera->GetShadowTranform());
        else per_pass_data.ShadowTransform = DirectX::XMMatrixIdentity();

        command_list.SetGraphicsDynamicConstantBuffer(to_underlying(RootParameters::PerPassData), per_pass_data);
    }

    if (m_dirty_flags & DF_PointLights) {
        command_list.SetGraphicsDynamicStructuredBuffer(to_underlying(RootParameters::PointLights), m_light_manager->GetPointLights());
    }

    if (m_dirty_flags & DF_SpotLights) {
        command_list.SetGraphicsDynamicStructuredBuffer(to_underlying(RootParameters::SpotLights), m_light_manager->GetSpotLights());
    }

    if (m_dirty_flags & DF_DirectionalLights) {
        command_list.SetGraphicsDynamicStructuredBuffer(to_underlying(RootParameters::DirectionalLights), m_light_manager->GetDirLights());
    }

    if (m_dirty_flags & (DF_PointLights | DF_SpotLights | DF_DirectionalLights)) {
        LightProperties light_props;
        light_props.NumPointLights = static_cast<uint32_t>(m_light_manager->GetPointLightsCount());
        light_props.NumSpotLights = static_cast<uint32_t>(m_light_manager->GetSpotLightsCount());
        light_props.NumDirectionalLights = static_cast<uint32_t>(m_light_manager->GetDirLightsCount());

        command_list.SetGraphics32BitConstants(to_underlying(RootParameters::LightPropertiesCB), light_props);
    }

    command_list.SetGraphics32BitConstants(to_underlying(RootParameters::FogPropertiesCB), m_fog_properties);

    const MeshManager::MeshMap& mesh_map = m_mesh_manager->GetMeshMap();
    for (const auto& [mesh_name, mesh_list] : mesh_map) {
        auto instance_buffer_view = m_mesh_manager->GetInstanceBufferView(mesh_name);
        //auto instance_data = m_mesh_manager->GetInstanceData(mesh_name);
        command_list.SetShaderResourceView(to_underlying(RootParameters::InstanceData), 0, instance_buffer_view);
        //command_list.SetGraphicsDynamicStructuredBuffer(to_underlying(RootParameters::InstanceIndexData), instance_data);

        const auto& instance_index_list = m_mesh_manager->GetInstanceIndexData(mesh_name);
        command_list.SetGraphicsDynamicStructuredBuffer(to_underlying(RootParameters::InstanceIndexData), instance_index_list);
        
        std::shared_ptr<MeshNode> mesh_node = mesh_list[0u];
        const MeshNode::MeshList& mesh_list = mesh_node->GetMeshes();
        for (const auto& mesh : mesh_list) {
            auto material = mesh->GetMaterial();
            if (material->IsTransparent()) continue;

            if (material) {
                auto& material_props = material->GetMaterialProperties();
                if (shadow_camera) {
                    material->SetTexture(Material::TextureType::Shadow, m_shadow_map_texture);
                }
                command_list.SetGraphicsDynamicConstantBuffer(to_underlying(RootParameters::MaterialCB), material_props);

                using TextureType = Material::TextureType;

                BindTexture(command_list, 0, material->GetTexture(TextureType::Ambient));
                BindTexture(command_list, 1, material->GetTexture(TextureType::Emissive));
                BindTexture(command_list, 2, material->GetTexture(TextureType::Diffuse));
                BindTexture(command_list, 3, material->GetTexture(TextureType::Specular));
                BindTexture(command_list, 4, material->GetTexture(TextureType::SpecularPower));
                BindTexture(command_list, 5, material->GetTexture(TextureType::Normal));
                BindTexture(command_list, 6, material->GetTexture(TextureType::Bump));
                BindTexture(command_list, 7, material->GetTexture(TextureType::Opacity));
                BindTexture(command_list, 8, material->GetTexture(TextureType::Displacement));
                BindTexture(command_list, 9, material->GetTexture(TextureType::Metalness));
                BindTexture(command_list, 10, material->GetTexture(TextureType::Shadow));
            }

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

void EffectInstancedPSO::SetFogProperties(const FogProperties& fog_props) {
    m_fog_properties = fog_props;
}

void EffectInstancedPSO::SetViewMatrix(const BasicCameraNode& camera) {
    m_pAligned_mvp->View = camera.GetView();
    m_pAligned_mvp->Projection = camera.GetProjection();
    m_near_z = camera.GetFrustum().Near;
    m_far_z = camera.GetFrustum().Far;
    m_dirty_flags |= DF_PerPassData | DF_Near_Far | DF_Near_Far;
}

void XM_CALLCONV EffectInstancedPSO::SetViewMatrix(DirectX::FXMMATRIX view_matrix) {
    m_pAligned_mvp->View = view_matrix;
    m_dirty_flags |= DF_PerPassData;
}

void XM_CALLCONV EffectInstancedPSO::SetProjectionMatrix(DirectX::FXMMATRIX projection_matrix) {
    m_pAligned_mvp->Projection = projection_matrix;
    m_dirty_flags |= DF_PerPassData;
}

void EffectInstancedPSO::SetNearZ(float near_z) {
    m_near_z = near_z;
    m_dirty_flags |= DF_Near_Far;
}

void EffectInstancedPSO::SetFarZ(float far_z) {
    m_far_z = far_z;
    m_dirty_flags |= DF_Near_Far;
}

void EffectInstancedPSO::SetRenderTargetSize(DirectX::XMFLOAT2 render_target_size) {
    m_render_target_size.x = render_target_size.x;
    m_render_target_size.y = render_target_size.y;
    m_render_target_size.z = 1.0f / render_target_size.x;
    m_render_target_size.w = 1.0f / render_target_size.y;
    m_dirty_flags |= DF_RT_Size;
}

void EffectInstancedPSO::SetShadowMapTexture(std::shared_ptr<Texture> shadow_map_texture) {
    m_shadow_map_texture = shadow_map_texture;
}
