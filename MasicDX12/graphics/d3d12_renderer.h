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
#include "../tools/com_exception.h"
#include "../graphics/adapter_reader.h"

#include "../graphics/imgui/imgui.h"
#include "../graphics/imgui/imgui_impl_win32.h"
#include "../graphics/imgui/imgui_impl_dx12.h"

using namespace std::literals;

class D3DRenderer12 : public IRenderer {
public:
	D3DRenderer12();

	virtual bool Initialize(const RenderWindow& rw) override;

	virtual void VSetBackgroundColor(DirectX::PackedVector::XMCOLOR color) override;
	virtual void VSetBackgroundColor4f(const DirectX::XMFLOAT4& color) override;
	virtual void VSetBackgroundColor3f(const DirectX::XMFLOAT3& color) override;
	virtual void VSetBackgroundColor(DirectX::FXMVECTOR color) override;
	
	virtual bool VPreRender() override;
	virtual bool VPresent() override;
	virtual bool VPostRender();

	//virtual void VPushRenderTarget(Microsoft::WRL::ComPtr<ID3D12Resource> render_target_view, Microsoft::WRL::ComPtr<ID3D12Resource> depth_stencil_view);
	//virtual void VPushRenderTarget(UINT NumViews, Microsoft::WRL::ComPtr<ID3D12Resource> render_target_view, Microsoft::WRL::ComPtr<ID3D12Resource> depth_stencil_view);
	virtual void VPopRenderTarget();

	virtual HRESULT VOnRestore(const RenderWindow& rw);
	virtual void VShutdown();
	
	ID3D12Device* GetDevice();
	//ID3D12DeviceContext* GetDeviceContext();
	IDXGISwapChain* GetSwapChain();

protected:
	std::array<float, 4> m_background_color;
	bool m_vsync_enabled;
	int m_video_card_memory;
	char m_video_card_description[128];

	bool GetAdapterInfo(const AdapterData& adapter);
	std::string print(D3D12_MESSAGE* msg);
	std::string print(DXGI_INFO_QUEUE_MESSAGE* msg);
	std::string printErrorMessages();

	void CreateCommandObjects();
	void CreateSwapChain(const RenderWindow& rw);
	void CreateRtvAndDsvDescriptorHeaps();

	void FlushCommandQueue();

	ID3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
	
#if defined(DEBUG) || defined(_DEBUG) 
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> m_d3d_info_queue;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_dxgi_info_queue;
	Microsoft::WRL::ComPtr<ID3D12Debug1> m_d3d_debug_controller;
	Microsoft::WRL::ComPtr<IDXGIDebug> m_dxgi_debug_controller;
#endif

	D3D_FEATURE_LEVEL m_d3d_feature_level = D3D_FEATURE_LEVEL_11_1;
	D3D_DRIVER_TYPE m_d3d_driver_type = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bool m_MSAA_state = false;
	UINT m_MSAA_sample_count = 4u;
	UINT m_MSAA_quality = 0u;

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgi_factory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_current_fence = 0ul;

	UINT m_rtv_desc_size = 0u;
	UINT m_dsv_desc_size = 0u;
	UINT m_cbv_srv_uav_desc_size = 0u;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_command_queue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_direct_cmd_list_alloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsv_heap;

	static const int m_swap_chain_buffer_count = 2u;
	int m_curr_back_buffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_swap_chain_buffers[m_swap_chain_buffer_count];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_depth_stencil_buffer;

	D3D12_VIEWPORT m_screen_viewport;
	D3D12_RECT m_scissor_rect;

	std::unique_ptr<DirectX::SpriteBatch> m_sprite_batch;
	std::unique_ptr<DirectX::SpriteFont> m_sprite_font;
};