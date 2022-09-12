#include "swap_chain.h"

#include "adapter_reader.h"
#include "command_list.h"
#include "command_queue.h"
#include "device.h"
#include "../gui.h"
#include "render_target.h"
#include "resource_state_tracker.h"
#include "texture.h"
#include "../tools/com_exception.h"

#include <algorithm>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

SwapChain::SwapChain(Device& device, HWND hWnd, DXGI_FORMAT render_target_format) : m_device(device), m_command_queue(device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)), m_hWnd(hWnd), m_fence_values{ 0 }, m_width(0u), m_height(0u), m_render_target_format(render_target_format), m_vsync(true), m_tearing_supported(false), m_fullscreen(false) {
    assert(hWnd);

    auto d3d12_command_queue = m_command_queue.GetD3D12CommandQueue();

    auto adapter = m_device.GetAdapter();
    auto dxgi_adapter = adapter->GetDXGIAdapter();

    Microsoft::WRL::ComPtr<IDXGIFactory> dxgi_factory;
    Microsoft::WRL::ComPtr<IDXGIFactory5> dxgi_factory5;
    HRESULT hr = dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
    ThrowIfFailed(hr);
    hr = dxgi_factory.As(&dxgi_factory5);
    ThrowIfFailed(hr);

    BOOL allow_tearing = FALSE;
    hr = dxgi_factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(BOOL));
    if (SUCCEEDED(hr)) {
        //m_tearing_supported = (allow_tearing == TRUE);
    }

    RECT window_rect;
    ::GetClientRect(hWnd, &window_rect);

    m_width = window_rect.right - window_rect.left;
    m_height = window_rect.bottom - window_rect.top;

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    swap_chain_desc.Width = m_width;
    swap_chain_desc.Height = m_height;
    swap_chain_desc.Format = m_render_target_format;
    swap_chain_desc.Stereo = FALSE;
    swap_chain_desc.SampleDesc = { 1, 0 };
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = BUFFER_COUNT;
    swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    swap_chain_desc.Flags = m_tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    swap_chain_desc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain1;
    hr = dxgi_factory5->CreateSwapChainForHwnd(d3d12_command_queue.Get(), m_hWnd, &swap_chain_desc, nullptr, nullptr, &dxgi_swap_chain1);
    ThrowIfFailed(hr);
    hr = dxgi_swap_chain1.As(&m_dxgi_swap_chain);
    ThrowIfFailed(hr);

    ThrowIfFailed(dxgi_factory5->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

    m_current_back_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();
    m_dxgi_swap_chain->SetMaximumFrameLatency(BUFFER_COUNT - 1);
    m_hFrame_latency_waitable_object = m_dxgi_swap_chain->GetFrameLatencyWaitableObject();

    UpdateRenderTargetViews();
}

SwapChain::~SwapChain() {}

void SwapChain::SetFullscreen(bool fullscreen) {
    if (m_fullscreen != fullscreen) {
        m_fullscreen = fullscreen;
    }
}

void SwapChain::WaitForSwapChain() {
    DWORD result = ::WaitForSingleObjectEx(m_hFrame_latency_waitable_object, 1000, TRUE);
}

void SwapChain::Resize(uint32_t width, uint32_t height) {
    if (m_width != width || m_height != height) {
        m_width = std::max(1u, width);
        m_height = std::max(1u, height);

        m_device.Flush();

        m_render_target.Reset();
        for (UINT i = 0; i < BUFFER_COUNT; ++i) {
            m_back_buffer_textures[i].reset();
        }

        DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
        HRESULT hr = m_dxgi_swap_chain->GetDesc(&swap_chain_desc);
        ThrowIfFailed(hr);
        hr = m_dxgi_swap_chain->ResizeBuffers(BUFFER_COUNT, m_width, m_height, swap_chain_desc.BufferDesc.Format, swap_chain_desc.Flags);
        ThrowIfFailed(hr);

        m_current_back_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();

        UpdateRenderTargetViews();
    }
}

uint32_t SwapChain::GetWidth() {
    return m_width;
}

uint32_t SwapChain::GetHeight() {
    return m_height;
}

const RenderTarget& SwapChain::GetRenderTarget() const {
    m_render_target.AttachTexture(AttachmentPoint::Color0, m_back_buffer_textures[m_current_back_buffer_index]);
    return m_render_target;
}

UINT SwapChain::Present(const std::shared_ptr<Texture>& texture) {
    auto command_list = m_command_queue.GetCommandList();
    auto back_buffer = m_back_buffer_textures[m_current_back_buffer_index];

    if (texture) {
        if (texture->GetD3D12ResourceDesc().SampleDesc.Count > 1) {
            command_list->ResolveSubresource(back_buffer, texture);
        }
        else {
            command_list->CopyResource(back_buffer, texture);
        }
    }

    command_list->TransitionBarrier(back_buffer, D3D12_RESOURCE_STATE_PRESENT);
    m_command_queue.ExecuteCommandList(command_list);

    UINT sync_interval = m_vsync ? 1 : 0;
    UINT present_flags = m_tearing_supported && !m_fullscreen && !m_vsync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    HRESULT hr = m_dxgi_swap_chain->Present(sync_interval, present_flags);
    ThrowIfFailed(hr);

    m_fence_values[m_current_back_buffer_index] = m_command_queue.Signal();
    m_current_back_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();

    auto fence_value = m_fence_values[m_current_back_buffer_index];
    m_command_queue.WaitForFenceValue(fence_value);
    m_device.ReleaseStaleDescriptors();

    return m_current_back_buffer_index;
}

void SwapChain::UpdateRenderTargetViews() {
    for (UINT i = 0; i < BUFFER_COUNT; ++i) {
        Microsoft::WRL::ComPtr<ID3D12Resource> back_buffer;
        HRESULT hr = m_dxgi_swap_chain->GetBuffer(i, IID_PPV_ARGS(back_buffer.GetAddressOf()));
        ThrowIfFailed(hr);

        ResourceStateTracker::AddGlobalResourceState(back_buffer.Get(), D3D12_RESOURCE_STATE_COMMON);

        m_back_buffer_textures[i] = m_device.CreateTexture(back_buffer);
        m_back_buffer_textures[i]->SetName(L"Backbuffer[" + std::to_wstring(i) + L"]");
    }
}

bool SwapChain::IsFullscreen() const {
    return m_fullscreen;
}

void SwapChain::ToggleFullscreen() {
    SetFullscreen(!m_fullscreen);
}

void SwapChain::SetVSync(bool vsync) {
    m_vsync = vsync;
}

bool SwapChain::GetVSync() const {
    return m_vsync;
}

void SwapChain::ToggleVSync() {
    SetVSync(!m_vsync);
}

bool SwapChain::IsTearingSupported() const {
    return m_tearing_supported;
}

DXGI_FORMAT SwapChain::GetRenderTargetFormat() const {
    return m_render_target_format;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> SwapChain::GetDXGISwapChain() const {
    return m_dxgi_swap_chain;
}
