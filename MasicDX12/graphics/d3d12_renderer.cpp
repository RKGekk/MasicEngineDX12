#include "d3d12_renderer.h"

#include <directx_old/d3dx12.h>

D3DRenderer12::D3DRenderer12() : m_vsync_enabled(true), m_tearing_supported(false) {}

bool D3DRenderer12::Initialize(const WindowSurface& rw) {
	m_adapter_reader = std::make_shared<AdapterReader>();
	m_adapter_reader->Initialize();

	//m_tearing_supported = m_adapter_reader->CheckTearingSupport();
	m_adapter = m_adapter_reader->GetAdapter();
	m_device = Device::Create(m_adapter);

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

HRESULT D3DRenderer12::VOnRestore(const WindowSurface& rw) {
	return S_OK;
}

void D3DRenderer12::VShutdown() {}

std::shared_ptr<Device> D3DRenderer12::GetDevice() {
	return m_device;
}

std::shared_ptr<SwapChain> D3DRenderer12::GetSwapChain() {
	return m_swap_chain;
}