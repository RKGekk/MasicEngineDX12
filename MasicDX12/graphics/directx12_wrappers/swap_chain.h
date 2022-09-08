#pragma once

#include "render_target.h"

#include <dxgi1_6.h>
#include <wrl.h>

#include <memory>

class CommandQueue;
class Device;
class Texture;

class SwapChain {
public:
	static const UINT BUFFER_COUNT = 3u;

	bool IsFullscreen() const;
	void SetFullscreen(bool fullscreen);
	void ToggleFullscreen();

	void SetVSync(bool vsync);
	bool GetVSync() const;
	void ToggleVSync();
	bool IsTearingSupported() const;

	void WaitForSwapChain();

	void Resize(uint32_t width, uint32_t height);
	uint32_t GetWidth();
	uint32_t GetHeight();
	DXGI_FORMAT GetRenderTargetFormat() const;

	const RenderTarget& GetRenderTarget() const;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> GetDXGISwapChain() const;

	UINT Present(const std::shared_ptr<Texture>& texture = nullptr);

protected:
	SwapChain(Device& device, HWND hWnd, DXGI_FORMAT render_target_format = DXGI_FORMAT_R8G8B8A8_UNORM);
	virtual ~SwapChain();

	void UpdateRenderTargetViews();

private:
	Device& m_device;

	CommandQueue& m_command_queue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgi_swap_chain;
	std::shared_ptr<Texture> m_back_buffer_textures[BUFFER_COUNT];
	mutable RenderTarget m_render_target;

	UINT m_current_back_buffer_index;
	UINT64 m_fence_values[BUFFER_COUNT];

	HANDLE m_hFrame_latency_waitable_object;
	HWND m_hWnd;

	uint32_t m_width;
	uint32_t m_height;

	DXGI_FORMAT m_render_target_format;

	bool m_vsync;
	bool m_tearing_supported = false;

	bool m_fullscreen;
};