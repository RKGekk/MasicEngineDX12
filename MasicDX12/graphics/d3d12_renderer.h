#pragma once

#include <memory>
#include <array>
#include <stack>
#include <utility>
#include <string>

#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <dxtk12/SpriteBatch.h>
#include <dxtk12/SpriteFont.h>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <dxgidebug.h>
#include <d3d12sdklayers.h>
#endif

#include "i_renderer.h"
#include "../window_surface.h"
#include "../tools/com_exception.h"
#include "directx12_wrappers/adapter_reader.h"
#include "directx12_wrappers/command_list.h"
#include "directx12_wrappers/device.h"
#include "directx12_wrappers/pipeline_state_object.h"
#include "directx12_wrappers/render_target.h"
#include "directx12_wrappers/root_signature.h"
#include "directx12_wrappers/swap_chain.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"

using namespace std::literals;

class D3DRenderer12 : public IRenderer {
public:
	D3DRenderer12();

	virtual bool Initialize(std::shared_ptr<WindowSurface> rw) override;

	virtual bool VPreRender() override;
	virtual bool VPresent() override;
	virtual bool VPostRender();

	virtual HRESULT VOnRestore() override;
	virtual void VShutdown();
	
	std::shared_ptr<Device> GetDevice();
	std::shared_ptr<SwapChain> GetSwapChain();

	virtual std::shared_ptr<WindowSurface> GetRenderWindow() override;
	DXGI_FORMAT GetBackBufferFormat();

protected:
	bool m_vsync_enabled;
	bool m_tearing_supported;

	std::shared_ptr<AdapterReader> m_adapter_reader;
	AdapterData::AdapterDataPtr m_adapter;
	std::shared_ptr<Device> m_device;
	std::shared_ptr<SwapChain> m_swap_chain;
	std::shared_ptr<WindowSurface> m_window_surface;
	DXGI_FORMAT m_back_buffer_format;
};