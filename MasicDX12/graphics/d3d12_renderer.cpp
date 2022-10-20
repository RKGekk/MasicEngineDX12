#include "d3d12_renderer.h"

#include <directx_old/d3dx12.h>

#include "../graphics/directx12_wrappers/texture.h"

D3DRenderer12::D3DRenderer12() : m_vsync_enabled(true), m_tearing_supported(false) {}

bool D3DRenderer12::Initialize(std::shared_ptr<WindowSurface> rw) {
	m_back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_back_buffer_clear_color = { 0.4f, 0.6f, 0.9f, 1.0f };
	m_window_surface = rw;
	m_adapter_reader = std::make_shared<AdapterReader>();
	m_adapter_reader->Initialize();

	//m_tearing_supported = m_adapter_reader->CheckTearingSupport();
	m_adapter = m_adapter_reader->GetAdapter();
	m_device = Device::Create(m_adapter);
	m_swap_chain = m_device->CreateSwapChain(m_window_surface->GetWindowHandle(), m_back_buffer_format);

	m_width = m_swap_chain->GetWidth();
	m_height = m_swap_chain->GetHeight();
	//m_sample_desc = device->GetMultisampleQualityLevels(m_back_buffer_format);
	m_sample_desc = { 1, 0 };
	
	m_depth_buffer_format = DXGI_FORMAT_D32_FLOAT;
	m_depth_clear_value = { 1.0f, 0 };

	D3D12_CLEAR_VALUE depth_clear_value = {};
	depth_clear_value.Format = m_depth_buffer_format;
	depth_clear_value.DepthStencil = m_depth_clear_value;
	auto depth_desc = CD3DX12_RESOURCE_DESC::Tex2D(m_depth_buffer_format, m_width, m_height, 1, 1, m_sample_desc.Count, m_sample_desc.Quality, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	auto depth_texture = m_device->CreateTexture(depth_desc, &depth_clear_value);
	depth_texture->SetName(L"Depth Render Target");

	m_render_target.AttachTexture(AttachmentPoint::Color0, m_swap_chain->GetRenderTarget().GetTexture(AttachmentPoint::Color0));
	m_render_target.AttachTexture(AttachmentPoint::DepthStencil, depth_texture);

	return true;
}

void D3DRenderer12::VSetClearColor(BYTE R, BYTE G, BYTE B, BYTE A) {
	m_back_buffer_clear_color.x = float(R) / 255.0f;
	m_back_buffer_clear_color.y = float(G) / 255.0f;
	m_back_buffer_clear_color.z = float(B) / 255.0f;
	m_back_buffer_clear_color.w = float(A) / 255.0f;
}

void D3DRenderer12::VSetClearColor4f(float R, float G, float B, float A) {
	m_back_buffer_clear_color.x = R;
	m_back_buffer_clear_color.y = G;
	m_back_buffer_clear_color.z = B;
	m_back_buffer_clear_color.w = A;
}

DirectX::XMFLOAT4 D3DRenderer12::VGetClearColor4f() {
	return m_back_buffer_clear_color;
}

bool D3DRenderer12::VPreRender(std::shared_ptr<CommandList> command_list) {
	m_render_target.AttachTexture(AttachmentPoint::Color0, m_swap_chain->GetRenderTarget().GetTexture(AttachmentPoint::Color0));
	command_list->ClearTexture(m_render_target.GetTexture(AttachmentPoint::Color0), &m_back_buffer_clear_color.x);
	command_list->ClearDepthStencilTexture(m_render_target.GetTexture(AttachmentPoint::DepthStencil), D3D12_CLEAR_FLAG_DEPTH);

	return true;
}

bool D3DRenderer12::VPresent() {
	m_swap_chain->Present();
	if (m_vsync_enabled) {
		m_swap_chain->WaitForSwapChain();
	}
	return true;
}

bool D3DRenderer12::VPostRender() {
	return true;
}

HRESULT D3DRenderer12::VOnRestore() {
	return S_OK;
}

void D3DRenderer12::VShutdown() {}

std::shared_ptr<Device> D3DRenderer12::GetDevice() {
	return m_device;
}

std::shared_ptr<SwapChain> D3DRenderer12::GetSwapChain() {
	return m_swap_chain;
}

std::shared_ptr<WindowSurface> D3DRenderer12::GetRenderWindow() {
	return m_window_surface;
}

DXGI_FORMAT D3DRenderer12::GetBackBufferFormat() {
	return m_back_buffer_format;
}

DXGI_FORMAT D3DRenderer12::GetDepthBufferFormat() {
	return m_depth_buffer_format;
}

RenderTarget& D3DRenderer12::GetRenderTarget() {
	return m_render_target;
}

uint32_t D3DRenderer12::GetRenderTargetWidth() {
	return m_width;
}

uint32_t D3DRenderer12::GetRenderTargetHeight() {
	return m_height;
}
