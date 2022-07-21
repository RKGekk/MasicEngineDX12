#include "shadow_manager.h"
#include "light_manager.h"
#include "scene.h"
#include "scene_node.h"
#include "../engine/engine.h"
#include "../graphics/d3d12_renderer.h"
#include "root_node.h"

#include <utility>

void ShadowManager::BuildShadowTransform(Scene* pScene) {
    using namespace DirectX;

    LightManager* light_manager = pScene->GetLightManager();
    ISceneNode* actor_render_pass = pScene->GetRootNode()->GetRenderPass(RenderPass::RenderPass_Actor);

    float scene_bounds_radius = actor_render_pass->VGet().Radius();
    //float scene_bounds_radius = 4.0f;
    XMFLOAT3 scene_bounds_center = actor_render_pass->VGet().Position3();

    const std::vector<DirectionalLight>& dl = light_manager->GetDirLights();
    XMVECTOR lightDir;
    if (dl.size() != 0u) {
        XMFLOAT3 dir = light_manager->GetDirLights()[0].Direction;
        //dir.z *= -1.0f;
        lightDir = XMLoadFloat3(&dir);
    }
    else {
        lightDir = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
    }
    XMVECTOR lightPos = -2.0f * scene_bounds_radius * lightDir;
    XMVECTOR targetPos = XMLoadFloat3(&scene_bounds_center);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

    XMFLOAT3 sphereCenterLS;
    XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

    float l = sphereCenterLS.x - scene_bounds_radius;
    float b = sphereCenterLS.y - scene_bounds_radius;
    float n = sphereCenterLS.z - scene_bounds_radius;
    float r = sphereCenterLS.x + scene_bounds_radius;
    float t = sphereCenterLS.y + scene_bounds_radius;
    float f = sphereCenterLS.z + scene_bounds_radius;
    XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

    XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f
    );

    XMMATRIX S = V * P * T;

    XMStoreFloat4x4(&m_light_view, V);
    XMStoreFloat4x4(&m_light_proj, P);
    XMStoreFloat4x4(&m_shadow_transform, S);
}

ShadowManager::ShadowManager() {
    DirectX::XMStoreFloat4x4(&m_light_view, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&m_light_proj, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&m_shadow_transform, DirectX::XMMatrixIdentity());

    D3DRenderer11* renderer = static_cast<D3DRenderer11*>(g_pApp->GetRenderer());
    ID3D11Device* device = renderer->GetDevice();

    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(ShadowManager::SMapSize);
    m_viewport.Height = static_cast<float>(ShadowManager::SMapSize);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width = ShadowManager::SMapSize;
    texDesc.Height = ShadowManager::SMapSize;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&texDesc, 0, m_depth_map.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create shadow buffer");

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = 0;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(m_depth_map.Get(), &dsvDesc, m_depth_map_dsv.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create shadow buffer view");

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = device->CreateShaderResourceView(m_depth_map.Get(), &srvDesc, m_depth_map_srv.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create shadow shader resource view");

    std::string vertex_shader_resource_name = "vertex_shader_shadow.cso";
    std::vector<D3D11_INPUT_ELEMENT_DESC> vertex_shader_input_layout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    m_vs = std::make_shared<VertexShader>();
    m_vs->Initialize(
        device,
        ShaderGonfig{}
        .set_shader_name(s2w(vertex_shader_resource_name))
        .set_description(std::move(vertex_shader_input_layout))
    );

    std::string pixel_shader_resource_name = "pixel_shader_shadow.cso";
    m_ps = std::make_shared<PixelShader>();
    m_ps->Initialize(
        device,
        ShaderGonfig{}
        .set_shader_name(s2w(pixel_shader_resource_name))
        .set_description({})
    );

    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 100000;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 1.0f;

    hr = device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state");

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    hr = device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state");

}

void ShadowManager::CalcShadow(Scene* pScene) {
    BuildShadowTransform(pScene);
    D3DRenderer11* renderer = static_cast<D3DRenderer11*>(g_pApp->GetRenderer());
    ID3D11DeviceContext* device_context = renderer->GetDeviceContext();

    device_context->ClearDepthStencilView(m_depth_map_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    renderer->VPushRenderTarget(m_render_target_view, m_depth_map_dsv);
    device_context->RSSetViewports(1, &m_viewport);
    device_context->VSSetShader(m_vs->GetShader(), nullptr, 0u);
    //device_context->PSSetShader(m_ps->GetShader(), nullptr, 0u);
    device_context->PSSetShader(nullptr, nullptr, 0u);
    device_context->RSSetState(m_rasterState.Get());
    //device_context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
    device_context->OMSetDepthStencilState(nullptr, 1);

    ISceneNode* actor_render_pass = pScene->GetRootNode()->GetRenderPass(RenderPass::RenderPass_Actor);
    actor_render_pass->VShadowRenderChildren(pScene);

    renderer->VPopRenderTarget();
}

ID3D11ShaderResourceView* const* ShadowManager::ShadowDepthMapSRVAddress() const {
    return m_depth_map_srv.GetAddressOf();
}

DirectX::XMMATRIX ShadowManager::GetLightView() {
    return DirectX::XMLoadFloat4x4(&m_light_view);
}

const DirectX::XMFLOAT4X4& ShadowManager::GetLightView4x4() {
    return m_light_view;
}

const DirectX::XMFLOAT4X4& ShadowManager::GetLightView4x4T() {
    DirectX::XMFLOAT4X4 res;
    DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetLightView()));
    return res;
}

DirectX::XMMATRIX ShadowManager::GetLightProj() {
    return DirectX::XMLoadFloat4x4(&m_light_proj);
}

const DirectX::XMFLOAT4X4& ShadowManager::GetLightProj4x4() {
    return m_light_proj;
}

const DirectX::XMFLOAT4X4& ShadowManager::GetLightProj4x4T() {
    DirectX::XMFLOAT4X4 res;
    DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetLightProj()));
    return res;
}

DirectX::XMMATRIX ShadowManager::GetShadowTransform() {
    return DirectX::XMLoadFloat4x4(&m_shadow_transform);
}

const DirectX::XMFLOAT4X4& ShadowManager::GetShadowTransform4x4() {
    return m_shadow_transform;
}

const DirectX::XMFLOAT4X4& ShadowManager::GetShadowTransform4x4T() {
    DirectX::XMFLOAT4X4 res;
    DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetShadowTransform()));
    return res;
}

DirectX::XMMATRIX ShadowManager::GetWorldShadowTransform(Scene* pScene) {
    DirectX::XMMATRIX world = pScene->GetTopMatrix();
    return DirectX::XMMatrixMultiply(world, GetShadowTransform());
    //return DirectX::XMMatrixMultiply(GetShadowTransform(), world);
}

DirectX::XMMATRIX ShadowManager::GetWorldShadowTransform(DirectX::XMMATRIX world) {
    return DirectX::XMMatrixMultiply(world, GetShadowTransform());
}

const DirectX::XMFLOAT4X4& ShadowManager::GetWorldShadowTransform4x4(Scene* pScene) {
    DirectX::XMFLOAT4X4 res;
    DirectX::XMMATRIX world = pScene->GetTopMatrix();
    DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixMultiply(world, GetShadowTransform()));
    //DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixMultiply(GetShadowTransform(), world));
    return res;
}

const DirectX::XMFLOAT4X4& ShadowManager::GetWorldShadowTransform4x4T(Scene* pScene) {
    DirectX::XMFLOAT4X4 res;
    DirectX::XMMATRIX world = pScene->GetTopMatrix();
    DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(world, GetShadowTransform())));
    //DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(GetShadowTransform(), world)));
    return res;
}

const DirectX::XMFLOAT4X4& ShadowManager::GetWorldShadowTransform4x4T(DirectX::XMMATRIX world) {
    DirectX::XMFLOAT4X4 res;
    DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(world, GetShadowTransform())));
    //DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(GetShadowTransform(), world)));
    return res;
}