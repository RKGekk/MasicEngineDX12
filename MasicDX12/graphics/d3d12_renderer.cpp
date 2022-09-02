#include "d3d12_renderer.h"

#include <directx_old/d3dx12.h>

D3DRenderer12::D3DRenderer12() : m_vsync_enabled(true), m_tearing_supported(false) {}

bool D3DRenderer12::Initialize(std::shared_ptr<WindowSurface> rw) {
	m_back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_window_surface = rw;
	m_adapter_reader = std::make_shared<AdapterReader>();
	m_adapter_reader->Initialize();

	//m_tearing_supported = m_adapter_reader->CheckTearingSupport();
	m_adapter = m_adapter_reader->GetAdapter();
	m_device = Device::Create(m_adapter);
	m_swap_chain = m_device->CreateSwapChain(m_window_surface->GetWindowHandle(), m_back_buffer_format);

	return true;
}

bool D3DRenderer12::VPreRender() {
	return true;
}

bool D3DRenderer12::VPresent() {
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
