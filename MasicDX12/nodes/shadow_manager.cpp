#include "shadow_manager.h"
#include "shadow_camera_node.h"
#include "../engine/engine.h"
#include "../graphics/d3d12_renderer.h"
#include "../graphics/directx12_wrappers/device.h"
#include "../graphics/directx12_wrappers/texture.h"

#include <directx_old/d3dx12.h>

ShadowManager::ShadowManager() : m_shadow_buffer_format(DXGI_FORMAT_D32_FLOAT) {
//ShadowManager::ShadowManager() : m_shadow_buffer_format(DXGI_FORMAT_R24G8_TYPELESS) {
	m_shadow_rt = std::make_shared<RenderTarget>();
}

void ShadowManager::AddShadow(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	std::shared_ptr<ShadowCameraNode> shadow_node = std::dynamic_pointer_cast<ShadowCameraNode>(node);
	if (shadow_node) {
		const std::string& name = shadow_node->Get().Name();
		m_shadow_map[name] = shadow_node;
		MakeRT();
	}
}

void ShadowManager::RemoveMesh(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	std::shared_ptr<ShadowCameraNode> shadow_node = std::dynamic_pointer_cast<ShadowCameraNode>(node);
	if (shadow_node) {
		const std::string& name = shadow_node->Get().Name();
		m_shadow_map.erase(name);
	}
}

const ShadowManager::ShadowMap& ShadowManager::GetShadowMap() const {
	return m_shadow_map;
}

int ShadowManager::GetShadowCount(ShadowName shadow_name) {
	return m_shadow_map.count(shadow_name);
}

std::shared_ptr<ShadowCameraNode> ShadowManager::GetShadow(ShadowName shadow_name) {
	return m_shadow_map[shadow_name];
}

std::shared_ptr<ShadowCameraNode> ShadowManager::GetShadow() {
	if (m_shadow_map.size()) {
		return (*m_shadow_map.begin()).second;
	}
	else {
		return std::shared_ptr<ShadowCameraNode>();
	}
}

DXGI_FORMAT ShadowManager::GetShadowBufferFormat() {
	return m_shadow_buffer_format;
}

std::shared_ptr<Texture> ShadowManager::GetShadowMapTexture() {
	//if (!m_shadow_map.size()) return std::shared_ptr<Texture>();

	//std::shared_ptr<Engine> engine = Engine::GetEngine();
	//std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());
	//std::shared_ptr<Device> device = renderer->GetDevice();

	//std::shared_ptr<ShadowCameraNode> shadow_camera = (*m_shadow_map.begin()).second;
	//const ShadowCameraNode::ShadowCameraProps& shadow_camera_props = shadow_camera->GetShadowProps();

	//if (!m_shadow_map_texture) {
	//	//DXGI_SAMPLE_DESC sample_desc = device->GetMultisampleQualityLevels(m_back_buffer_format);
	//	DXGI_SAMPLE_DESC sample_desc = { 1, 0 };
	//	auto depth_desc = CD3DX12_RESOURCE_DESC::Tex2D(m_shadow_buffer_format, shadow_camera_props.ShadowMapWidth, shadow_camera_props.ShadowMapHeight, 1, 1, sample_desc.Count, sample_desc.Quality);
	//	D3D12_CLEAR_VALUE shadow_clear_value = {};
	//	shadow_clear_value.Format = depth_desc.Format;
	//	shadow_clear_value.DepthStencil = { 1.0f, 0 };

	//	m_shadow_map_texture = device->CreateTexture(depth_desc, &shadow_clear_value);
	//	m_shadow_map_texture->SetName(L"Shadow Texture");
	//}


	return m_shadow_rt->GetTexture(AttachmentPoint::DepthStencil);
}

D3D12_VIEWPORT ShadowManager::GetShadowViewport() {
	return m_viewport;
}

D3D12_RECT ShadowManager::GetShadowRect() {
	return m_scissor_rect;
}

std::shared_ptr<RenderTarget> ShadowManager::GetRT() {
	return m_shadow_rt;
}

void ShadowManager::MakeRT() {
	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());
	std::shared_ptr<Device> device = renderer->GetDevice();

	std::shared_ptr<ShadowCameraNode> shadow_camera = GetShadow();
	const ShadowCameraNode::ShadowCameraProps& shadow_camera_props = shadow_camera->GetShadowProps();
	DXGI_SAMPLE_DESC sample_desc = { 1, 0 };
	auto shadow_rt_desc = CD3DX12_RESOURCE_DESC::Tex2D(
		m_shadow_buffer_format,
		shadow_camera_props.ShadowMapWidth,
		shadow_camera_props.ShadowMapHeight,
		1,
		1,
		sample_desc.Count,
		sample_desc.Quality,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	D3D12_CLEAR_VALUE depth_clear_value = {};
	depth_clear_value.Format = shadow_rt_desc.Format;
	//depth_clear_value.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_clear_value.DepthStencil = { 1.0f, 0 };
	//D3D12_RESOURCE_DESC shadow_rt_desc;
	//ZeroMemory(&shadow_rt_desc, sizeof(D3D12_RESOURCE_DESC));
	//shadow_rt_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//shadow_rt_desc.Alignment = 0;
	//shadow_rt_desc.Width = shadow_camera_props.ShadowMapWidth;
	//shadow_rt_desc.Height = shadow_camera_props.ShadowMapHeight;
	//shadow_rt_desc.DepthOrArraySize = 1;
	//shadow_rt_desc.MipLevels = 1;
	//shadow_rt_desc.Format = mFormat;
	//shadow_rt_desc.SampleDesc.Count = 1;
	//shadow_rt_desc.SampleDesc.Quality = 0;
	//shadow_rt_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//shadow_rt_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	//
	//D3D12_CLEAR_VALUE optClear;
	//optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//optClear.DepthStencil.Depth = 1.0f;
	//optClear.DepthStencil.Stencil = 0;

	auto shadow_rt_texture = device->CreateTexture(shadow_rt_desc, &depth_clear_value);
	shadow_rt_texture->SetName(L"Shadow Render Target");

	m_shadow_rt->AttachTexture(AttachmentPoint::DepthStencil, shadow_rt_texture);

	m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(shadow_camera_props.ShadowMapWidth), static_cast<float>(shadow_camera_props.ShadowMapHeight));
	m_scissor_rect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
}
