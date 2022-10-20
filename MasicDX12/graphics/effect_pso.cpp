#include "effect_pso.h"

#include "directx12_wrappers/command_list.h"
#include "directx12_wrappers/device.h"
#include "material.h"
#include "directx12_wrappers/pipeline_state_object.h"
#include "directx12_wrappers/root_signature.h"
#include "directx12_wrappers/texture.h"
#include "vertex_types.h"
#include "../tools/com_exception.h"
#include "../tools/string_utility.h"
#include "../nodes/light_manager.h"
#include "../tools/memory_utility.h"

#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include <wrl/client.h>

#include <array>

EffectPSO::EffectPSO(std::shared_ptr<Device> device, bool enable_lighting, bool enable_decal) : m_device(device), m_dirty_flags(DF_All), m_pPrevious_command_list(nullptr), m_enable_lighting(enable_lighting), m_enable_decal(enable_decal), m_need_transpose(true) {
    using namespace std::literals;
    m_pAligned_mvp = (MVP*)_aligned_malloc(sizeof(MVP), 16);

    Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader_blob;
    HRESULT hr = D3DReadFileToBlob(L"Basic_VS.cso", vertex_shader_blob.GetAddressOf());
    ThrowIfFailed(hr);
    m_vertex_shader = std::make_shared<VertexShader>(vertex_shader_blob, "main"s, "Basic_VS.cso"s);

    Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader_blob;
    std::string pixel_shader_name = ""s;
    if (enable_lighting)
        if (enable_decal) pixel_shader_name = "Decal_PS.cso";
        else pixel_shader_name = "Lighting_PS.cso";
    else pixel_shader_name = "Unlit_PS.cso";
    hr = D3DReadFileToBlob(to_wstring(pixel_shader_name).c_str(), pixel_shader_blob.GetAddressOf());
    ThrowIfFailed(hr);
    m_pixel_shader = std::make_shared<PixelShader>(pixel_shader_blob, "main"s, pixel_shader_name);

    D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
    CD3DX12_DESCRIPTOR_RANGE1 descriptor_rage(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, to_underlying(Material::TextureType::NumTypes), 3);

    CD3DX12_ROOT_PARAMETER1 root_parameters[RootParameters::NumRootParameters];
    root_parameters[RootParameters::MatricesCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    root_parameters[RootParameters::MaterialCB].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[RootParameters::LightPropertiesCB].InitAsConstants(sizeof(LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[RootParameters::FogPropertiesCB].InitAsConstants(sizeof(FogProperties) / 4, 2, 0, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[RootParameters::PointLights].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[RootParameters::SpotLights].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[RootParameters::DirectionalLights].InitAsShaderResourceView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameters[RootParameters::Textures].InitAsDescriptorTable(1, &descriptor_rage, D3D12_SHADER_VISIBILITY_PIXEL);

    const CD3DX12_STATIC_SAMPLER_DESC anisotropic_sampler(0, D3D12_FILTER_ANISOTROPIC);
    const CD3DX12_STATIC_SAMPLER_DESC shadow_sampler(1, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> samplers = { anisotropic_sampler, shadow_sampler };

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description;
    root_signature_description.Init_1_1(RootParameters::NumRootParameters, root_parameters, samplers.size(), samplers.data(), root_signature_flags);

    m_root_signature = m_device->CreateRootSignature("RootSignFor"s + pixel_shader_name, root_signature_description);

    //DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT depth_buffer_format = DXGI_FORMAT_D32_FLOAT;

    //DXGI_SAMPLE_DESC sample_desc = m_device->GetMultisampleQualityLevels(back_buffer_format);
    DXGI_SAMPLE_DESC sample_desc = { 1, 0 };

    D3D12_RT_FORMAT_ARRAY rtv_formats = {};
    rtv_formats.NumRenderTargets = 1;
    rtv_formats.RTFormats[0] = back_buffer_format;

    CD3DX12_RASTERIZER_DESC rasterizer_state(D3D12_DEFAULT);
    if (m_enable_decal) {
        rasterizer_state.CullMode = D3D12_CULL_MODE_NONE;
    }

    m_vertex_shader->AddRegister({ 0, 0, ShaderRegister::ConstantBuffer }, "MatricesCB"s);
    m_vertex_shader->SetInputAssemblerLayout(VertexPositionNormalTangentBitangentTexture::InputLayout);
    m_vertex_shader->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

    m_pixel_shader->AddRegister({ 0, 1, ShaderRegister::ConstantBuffer }, "MaterialCB"s);
    m_pixel_shader->AddRegister({ 1, 0, ShaderRegister::ConstantBuffer }, "LightPropertiesCB");
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
    m_pixel_shader->SetRenderTargetFormat(AttachmentPoint::DepthStencil, depth_buffer_format);
    //m_pixel_shader->SetBlendState();
    m_pixel_shader->SetRasterizerState(rasterizer_state);
    //m_pixel_shader->SetDepthStencilState();
    m_pixel_shader->SetSample(sample_desc);
    
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

EffectPSO::~EffectPSO() {
    _aligned_free(m_pAligned_mvp);
}

void EffectPSO::SetLightManager(std::shared_ptr<LightManager> light_manager) {
    m_light_manager = light_manager;
    m_dirty_flags |= DF_PointLights | DF_SpotLights | DF_DirectionalLights;
}

inline void EffectPSO::BindTexture(CommandList& command_list, uint32_t offset, const std::shared_ptr<Texture>& texture) {
    if (texture) {
        command_list.SetShaderResourceView(RootParameters::Textures, offset, texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
    else {
        command_list.SetShaderResourceView(RootParameters::Textures, offset, m_default_srv, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void EffectPSO::Apply(CommandList& command_list) {
    command_list.SetPipelineState(m_pipeline_state_object);
    command_list.SetGraphicsRootSignature(m_root_signature);

    if (m_dirty_flags & DF_Matrices) {
        Matrices m;
        m.ModelMatrix = m_pAligned_mvp->World;
        m.ModelViewMatrix = m_pAligned_mvp->World * m_pAligned_mvp->View;
        m.ModelViewProjectionMatrix = m.ModelViewMatrix * m_pAligned_mvp->Projection;
        m.InverseTransposeModelViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m.ModelViewMatrix));
        m.ShadowMatrix = m_pAligned_mvp->Shadow;

        command_list.SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, m);
    }

    if (m_dirty_flags & DF_Material) {
        if (m_material) {
            const auto& material_props = m_material->GetMaterialProperties();

            command_list.SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, material_props);

            using TextureType = Material::TextureType;

            BindTexture(command_list, 0, m_material->GetTexture(TextureType::Ambient));
            BindTexture(command_list, 1, m_material->GetTexture(TextureType::Emissive));
            BindTexture(command_list, 2, m_material->GetTexture(TextureType::Diffuse));
            BindTexture(command_list, 3, m_material->GetTexture(TextureType::Specular));
            BindTexture(command_list, 4, m_material->GetTexture(TextureType::SpecularPower));
            BindTexture(command_list, 5, m_material->GetTexture(TextureType::Normal));
            BindTexture(command_list, 6, m_material->GetTexture(TextureType::Bump));
            BindTexture(command_list, 7, m_material->GetTexture(TextureType::Opacity));
            BindTexture(command_list, 8, m_material->GetTexture(TextureType::Displacement));
            BindTexture(command_list, 9, m_material->GetTexture(TextureType::Metalness));
            BindTexture(command_list, 10, m_material->GetTexture(TextureType::Shadow));
        }
    }

    if (m_dirty_flags & DF_PointLights) {
        command_list.SetGraphicsDynamicStructuredBuffer(RootParameters::PointLights, m_light_manager->GetPointLights());
    }

    if (m_dirty_flags & DF_SpotLights) {
        command_list.SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLights, m_light_manager->GetSpotLights());
    }

    if (m_dirty_flags & DF_DirectionalLights) {
        command_list.SetGraphicsDynamicStructuredBuffer(RootParameters::DirectionalLights, m_light_manager->GetDirLights());
    }

    if (m_dirty_flags & (DF_PointLights | DF_SpotLights | DF_DirectionalLights)) {
        LightProperties light_props;
        light_props.NumPointLights = static_cast<uint32_t>(m_light_manager->GetPointLightsCount());
        light_props.NumSpotLights = static_cast<uint32_t>(m_light_manager->GetSpotLightsCount());
        light_props.NumDirectionalLights = static_cast<uint32_t>(m_light_manager->GetDirLightsCount());

        command_list.SetGraphics32BitConstants(RootParameters::LightPropertiesCB, light_props);
    }

    command_list.SetGraphics32BitConstants(to_underlying(RootParameters::FogPropertiesCB), m_fog_properties);

    m_dirty_flags = DF_None;
}

void EffectPSO::SetMaterial(const std::shared_ptr<Material>& material) {
    m_material = material;
    m_dirty_flags |= DF_Material;
}

void EffectPSO::SetFogProperties(const FogProperties& fog_props) {
    m_fog_properties = fog_props;
}

void XM_CALLCONV EffectPSO::SetWorldMatrix(DirectX::FXMMATRIX world_matrix) {
    m_pAligned_mvp->World = world_matrix;
    m_dirty_flags |= DF_Matrices;
}

void XM_CALLCONV EffectPSO::SetViewMatrix(DirectX::FXMMATRIX view_matrix) {
    m_pAligned_mvp->View = view_matrix;
    m_dirty_flags |= DF_Matrices;
}

void XM_CALLCONV EffectPSO::SetProjectionMatrix(DirectX::FXMMATRIX projection_matrix) {
    m_pAligned_mvp->Projection = projection_matrix;
    m_dirty_flags |= DF_Matrices;
}

void XM_CALLCONV EffectPSO::SetShadowMatrix(DirectX::FXMMATRIX shadow_matrix) {
    m_pAligned_mvp->Shadow = shadow_matrix;
    m_dirty_flags |= DF_Matrices;
}

void EffectPSO::AddShadowTexture(std::shared_ptr<Texture> shadow_texture) {
    m_material->SetTexture(Material::TextureType::Shadow, shadow_texture);
}