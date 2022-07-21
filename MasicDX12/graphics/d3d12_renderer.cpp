#include "d3d12_renderer.h"

#include <directx_old/d3dx12.h>

D3DRenderer12::D3DRenderer12() {}

bool D3DRenderer12::Initialize(const RenderWindow& rw) {
	if (!DirectX::XMVerifyCPUSupport()) { return false; }

	UINT createDeviceFlags = 0;
	HRESULT hr;

#if defined(DEBUG) || defined(_DEBUG) 
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(m_d3d_debug_controller.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to get d3d12 debug interface.");
	m_d3d_debug_controller->EnableDebugLayer();
	//m_d3d_debug_controller->SetEnableGPUBasedValidation(true);

	hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_dxgi_factory.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create DXGIFactory for enumerating adapters.");
#else
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));
#endif

	m_vsync_enabled = !rw.GetConfig().options.m_run_full_speed;
	
	std::vector<AdapterData>& adapters = AdapterReader::GetAdapterData(m_dxgi_factory);
	if (adapters.size() == 0u) return false; // No IDXGI Adapters found.
	const AdapterData& adapter = adapters[0];
	if (!GetAdapterInfo(adapter)) return false;

	hr = D3D12CreateDevice(adapter.m_pAdapter.Get(), m_d3d_feature_level, IID_PPV_ARGS(m_device.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create the swap chain, Direct3D device, and Direct3D device context.");

#if defined(DEBUG) || defined(_DEBUG)
	COM_ERROR_IF_FAILED(m_device->QueryInterface(IID_PPV_ARGS(m_d3d_info_queue.GetAddressOf())), "");
	COM_ERROR_IF_FAILED(DXGIGetDebugInterface1(0u, IID_PPV_ARGS(m_dxgi_debug_controller.GetAddressOf())), "");
	COM_ERROR_IF_FAILED(DXGIGetDebugInterface1(0u, IID_PPV_ARGS(m_dxgi_info_queue.GetAddressOf())), "");
#endif

	hr = m_device->CreateFence(0ul, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	COM_ERROR_IF_FAILED(hr, "Failed to create main sync fence.");


	m_rtv_desc_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsv_desc_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbv_srv_uav_desc_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ms_quality_levels;
	ms_quality_levels.Format = m_back_buffer_format;
	ms_quality_levels.SampleCount = m_MSAA_sample_count;
	ms_quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	ms_quality_levels.NumQualityLevels = 0u;
	hr = m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &ms_quality_levels, sizeof(ms_quality_levels));
	COM_ERROR_IF_FAILED(hr, "Failed to retrieve mulisample quality.");

	m_MSAA_state = ms_quality_levels.NumQualityLevels > 0u;
	m_MSAA_quality = ms_quality_levels.NumQualityLevels;

	CreateCommandObjects();
	CreateSwapChain(rw);
	CreateRtvAndDsvDescriptorHeaps();

	//DirectX::SpriteBatchPipelineStateDescription pd(rtState);
	//m_sprite_batch = std::make_unique<DirectX::SpriteBatch>(m_device, upload, pd);
	//m_sprite_font = std::make_unique<DirectX::SpriteFont>(m_device, upload, L"myfont.spritefont", m_resourceDescriptors->GetCpuHandle(Descriptors::TextFont), m_resourceDescriptors->GetGpuHandle(Descriptors::TextFont));
	//m_sprite_batch = std::make_unique<DirectX::SpriteBatch>(m_device_context.Get());
	//m_sprite_font = std::make_unique<DirectX::SpriteFont>(m_device.Get(), L"data/fonts/comic_sans_ms_16.sprite");

	return true;
}

void D3DRenderer12::VSetBackgroundColor(DirectX::PackedVector::XMCOLOR color) {
	m_background_color[0] = float(color.r) / 255.0f;
	m_background_color[1] = float(color.g) / 255.0f;
	m_background_color[2] = float(color.b) / 255.0f;
	m_background_color[3] = float(color.a) / 255.0f;
}

void D3DRenderer12::VSetBackgroundColor4f(const DirectX::XMFLOAT4& color) {
	m_background_color[0] = color.x;
	m_background_color[1] = color.y;
	m_background_color[2] = color.z;
	m_background_color[3] = color.w;
}

void D3DRenderer12::VSetBackgroundColor3f(const DirectX::XMFLOAT3& color) {
	m_background_color[0] = color.x;
	m_background_color[1] = color.y;
	m_background_color[2] = color.z;
	m_background_color[3] = 1.0f;
}

void D3DRenderer12::VSetBackgroundColor(DirectX::FXMVECTOR color) {
	m_background_color[0] = DirectX::XMVectorGetX(color);
	m_background_color[1] = DirectX::XMVectorGetY(color);
	m_background_color[2] = DirectX::XMVectorGetZ(color);
	m_background_color[3] = DirectX::XMVectorGetW(color);
}

bool D3DRenderer12::VPreRender() {
	//VPushRenderTarget(m_render_target_view, m_depth_stencil_view);

	//m_device_context->ClearRenderTargetView(m_render_target_view.Get(), m_backgroundColor.data());
	//m_device_context->ClearDepthStencilView(m_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();

	return true;
}

bool D3DRenderer12::VPresent() {
	//ImGui::Render();
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//m_swap_chain->Present(1, NULL);
	return true;
}

bool D3DRenderer12::VPostRender() {
	VPopRenderTarget();
	return true;
}

//void D3DRenderer12::VPushRenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view) {
//	m_render_targer_stack.push({ render_target_view, depth_stencil_view });
//	m_device_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
//}

//void D3DRenderer12::VPushRenderTarget(UINT NumViews, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view) {
//	m_render_targer_stack.push({ render_target_view, depth_stencil_view });
//	m_device_context->OMSetRenderTargets(NumViews, render_target_view.GetAddressOf(), depth_stencil_view.Get());
//}

void D3DRenderer12::VPopRenderTarget() {
	//m_render_targer_stack.pop();
	//if (m_render_targer_stack.size()) {
	//	std::pair<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, Microsoft::WRL::ComPtr<ID3D11DepthStencilView>>& prev_render_target = m_render_targer_stack.top();
	//	m_device_context->OMSetRenderTargets(1, prev_render_target.first.GetAddressOf(), prev_render_target.second.Get());
	//	m_device_context->RSSetViewports(1, &m_viewport);
	//}
}

HRESULT D3DRenderer12::VOnRestore(const RenderWindow& rw) {
	HRESULT hr;
	FlushCommandQueue();

	hr = m_command_list->Reset(m_direct_cmd_list_alloc.Get(), nullptr);
	COM_ERROR_IF_FAILED(hr, "Failed to reset graphics command list.");

	for (int i = 0; i < m_swap_chain_buffer_count; ++i) m_swap_chain_buffers[i].Reset();
	m_depth_stencil_buffer.Reset();

	long client_width = rw.GetWidth();
	long client_height = rw.GetHeight();
	hr = m_swap_chain->ResizeBuffers(m_swap_chain_buffer_count, client_width, client_height, m_back_buffer_format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	COM_ERROR_IF_FAILED(hr, "Failed to resize back buffer.");

	m_curr_back_buffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < m_swap_chain_buffer_count; i++) {
		hr = m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_swap_chain_buffers[i]));
		COM_ERROR_IF_FAILED(hr, "Failed to get back buffers resource.");

		m_device->CreateRenderTargetView(m_swap_chain_buffers[i].Get(), nullptr, rtv_heap_handle);
		rtv_heap_handle.Offset(1, m_rtv_desc_size);
	}

	D3D12_RESOURCE_DESC depth_stencil_desc;
	depth_stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depth_stencil_desc.Alignment = 0;
	depth_stencil_desc.Width = client_width;
	depth_stencil_desc.Height = client_height;
	depth_stencil_desc.DepthOrArraySize = 1;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depth_stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE opt_clear;
	opt_clear.Format = m_depth_stencil_format;
	opt_clear.DepthStencil.Depth = 1.0f;
	opt_clear.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	hr = m_device->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE, &depth_stencil_desc, D3D12_RESOURCE_STATE_COMMON, &opt_clear, IID_PPV_ARGS(m_depth_stencil_buffer.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create committed depth resource.");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
	dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Format = m_depth_stencil_format;
	dsv_desc.Texture2D.MipSlice = 0;
	m_device->CreateDepthStencilView(m_depth_stencil_buffer.Get(), &dsv_desc, GetDepthStencilView());

	D3D12_RESOURCE_BARRIER resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_command_list->ResourceBarrier(1u, &resource_barrier);
	hr = m_command_list->Close();
	COM_ERROR_IF_FAILED(hr, "Failed to execute/close resize commands.");
	std::array<ID3D12CommandList*, 1> cmds_lists = { m_command_list.Get() };
	m_command_queue->ExecuteCommandLists(static_cast<UINT>(cmds_lists.size()), cmds_lists.data());

	FlushCommandQueue();

	m_screen_viewport.TopLeftX = 0;
	m_screen_viewport.TopLeftY = 0;
	m_screen_viewport.Width = static_cast<float>(client_width);
	m_screen_viewport.Height = static_cast<float>(client_height);
	m_screen_viewport.MinDepth = 0.0f;
	m_screen_viewport.MaxDepth = 1.0f;

	m_scissor_rect = { 0, 0, client_width, client_height };

	return S_OK;
}

void D3DRenderer12::VShutdown() {}

ID3D12Device* D3DRenderer12::GetDevice() {
	return m_device.Get();
}

//ID3D11DeviceContext* D3DRenderer12::GetDeviceContext() {
//	return m_device_context.Get();
//}

IDXGISwapChain* D3DRenderer12::GetSwapChain() {
	return m_swap_chain.Get();
}

bool D3DRenderer12::GetAdapterInfo(const AdapterData& adapter) {
	m_video_card_memory = (int)(adapter.m_description.DedicatedVideoMemory / 1024 / 1024);
	unsigned long long stringLength;
	int error = wcstombs_s(&stringLength, m_video_card_description, 128, adapter.m_description.Description, 128);
	if (error != 0) { return false; }
	return true;
}

std::string D3DRenderer12::print(D3D12_MESSAGE* msg) {
	std::string categoryName = "(Unknown)";
	switch (msg->Category) {
		case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED:
			categoryName = "Application defined"; break;
		case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS:
			categoryName = "Miscellaneous"; break;
		case D3D12_MESSAGE_CATEGORY_INITIALIZATION:
			categoryName = "Initialization"; break;
		case D3D12_MESSAGE_CATEGORY_CLEANUP:
			categoryName = "Cleanup"; break;
		case D3D12_MESSAGE_CATEGORY_COMPILATION:
			categoryName = "Compilation"; break;
		case D3D12_MESSAGE_CATEGORY_STATE_CREATION:
			categoryName = "State creation"; break;
		case D3D12_MESSAGE_CATEGORY_STATE_SETTING:
			categoryName = "State setting"; break;
		case D3D12_MESSAGE_CATEGORY_STATE_GETTING:
			categoryName = "State getting"; break;
		case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
			categoryName = "Resource manipulation"; break;
		case D3D12_MESSAGE_CATEGORY_EXECUTION:
			categoryName = "Execution"; break;
		case D3D12_MESSAGE_CATEGORY_SHADER:
			categoryName = "Shader"; break;
	}

	std::string severityName = "(Unknown)";
	switch (msg->Severity) {
		case D3D12_MESSAGE_SEVERITY_CORRUPTION:
			severityName = "Corruption"; break;
		case D3D12_MESSAGE_SEVERITY_ERROR:
			severityName = "Error"; break;
		case D3D12_MESSAGE_SEVERITY_WARNING:
			severityName = "Warning"; break;
		case D3D12_MESSAGE_SEVERITY_INFO:
			severityName = "Info"; break;
		case D3D12_MESSAGE_SEVERITY_MESSAGE:
			severityName = "Message"; break;
	}

	return "Direct3D12 Debug Message (Category: "s + categoryName + ", Severity: "s + severityName + ", ID: " + std::to_string(msg->ID) + ") :\n"s + std::string(msg->pDescription);
}

std::string D3DRenderer12::print(DXGI_INFO_QUEUE_MESSAGE* msg) {
	std::string categoryName = "(Unknown)";
	switch (msg->Category) {
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_MISCELLANEOUS:
			categoryName = "Miscellaneous"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_INITIALIZATION:
			categoryName = "Initialization"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_CLEANUP:
			categoryName = "Cleanup"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_COMPILATION:
			categoryName = "Compilation"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_CREATION:
			categoryName = "State creation"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_SETTING:
			categoryName = "State setting"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_GETTING:
			categoryName = "State getting"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
			categoryName = "Resource manipulation"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_EXECUTION:
			categoryName = "Execution"; break;
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_SHADER:
			categoryName = "Shader"; break;
	}

	std::string severityName = "(Unknown)";
	switch (msg->Severity) {
		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
			severityName = "Corruption"; break;
		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
			severityName = "Error"; break;
		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
			severityName = "Warning"; break;
		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
			severityName = "Info"; break;
		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
			severityName = "Message"; break;
	}

	OLECHAR* producerStr;
	COM_ERROR_IF_FAILED(StringFromCLSID(msg->Producer, &producerStr), "");

	std::string res = "DXGI Debug Message (Category: "s + categoryName + ", Severity: "s + severityName + ", Producer: {"s + w2s(std::wstring(producerStr)) + "}, ID: "s + std::to_string(msg->ID) + ") :\n"s + std::string(msg->pDescription);

	CoTaskMemFree(producerStr);
	return res;
}

std::string D3DRenderer12::printErrorMessages() {
	std::string res;

	HRESULT hr;
	if (!m_d3d_info_queue) return ""s;

	// Direct3D messages
	UINT64 d3dCount = m_d3d_info_queue->GetNumStoredMessagesAllowedByRetrievalFilter();
	for (UINT64 i = 0; i < d3dCount; i++) {
		SIZE_T msgSize = 0;
		hr = m_d3d_info_queue->GetMessage(i, nullptr, &msgSize);

		std::vector<unsigned char> buffer(msgSize);
		auto* msg = reinterpret_cast<D3D12_MESSAGE*>(buffer.data());
		hr = m_d3d_info_queue->GetMessage(i, msg, &msgSize);

		res += print(msg);
	}

	// DXGI messages
	UINT64 dxgiCount = m_dxgi_info_queue->GetNumStoredMessagesAllowedByRetrievalFilters(DXGI_DEBUG_ALL);
	for (UINT64 i = 0; i < dxgiCount; i++) {
		SIZE_T msgSize = 0;
		hr = m_dxgi_info_queue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &msgSize);

		std::vector<unsigned char> buffer(msgSize);
		auto* msg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(buffer.data());
		hr = m_dxgi_info_queue->GetMessage(DXGI_DEBUG_ALL, i, msg, &msgSize);
		res += print(msg);
	}

	return res;
}

void D3DRenderer12::CreateCommandObjects() {
	HRESULT hr;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_command_queue.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create the command queue.");

	hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_direct_cmd_list_alloc.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create direct type command list allocator.");

	hr = m_device->CreateCommandList(0u, D3D12_COMMAND_LIST_TYPE_DIRECT, m_direct_cmd_list_alloc.Get(), nullptr, IID_PPV_ARGS(m_command_list.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create direct type command list.");

	m_command_list->Close();
}

void D3DRenderer12::CreateSwapChain(const RenderWindow& rw) {
	m_swap_chain.Reset();

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	swap_chain_desc.BufferDesc.Width = rw.GetWidth();
	swap_chain_desc.BufferDesc.Height = rw.GetHeight();
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60u;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1u;
	swap_chain_desc.BufferDesc.Format = m_back_buffer_format;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SampleDesc.Count = 1u;
	swap_chain_desc.SampleDesc.Quality = 0u;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = m_swap_chain_buffer_count;
	swap_chain_desc.OutputWindow = rw.GetHWND();
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	HRESULT hr = m_dxgi_factory->CreateSwapChain(m_command_queue.Get(), &swap_chain_desc, m_swap_chain.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create swap chain.");
}

void D3DRenderer12::CreateRtvAndDsvDescriptorHeaps() {
	HRESULT hr;

	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
	rtv_heap_desc.NumDescriptors = m_swap_chain_buffer_count;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtv_heap_desc.NodeMask = 0u;
	hr = m_device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(m_rtv_heap.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create rtv descriptor heap.");

	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
	dsv_heap_desc.NumDescriptors = 1u;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsv_heap_desc.NodeMask = 0u;
	hr = m_device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(m_dsv_heap.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create dsv descriptor heap.");
}

void D3DRenderer12::FlushCommandQueue() {
	m_current_fence++;

	HRESULT hr = m_command_queue->Signal(m_fence.Get(), m_current_fence);
	COM_ERROR_IF_FAILED(hr, "Failed to create signal fence.");

	UINT64 completed_value = m_fence->GetCompletedValue();
	if (completed_value < m_current_fence) {
		HANDLE event_handle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		hr = m_fence->SetEventOnCompletion(m_current_fence, event_handle);
		WaitForSingleObject(event_handle, INFINITE);
		CloseHandle(event_handle);
	}
}

ID3D12Resource* D3DRenderer12::GetCurrentBackBuffer() const {
	return m_swap_chain_buffers[m_curr_back_buffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DRenderer12::GetCurrentBackBufferView() const {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtv_heap->GetCPUDescriptorHandleForHeapStart(), m_curr_back_buffer, m_rtv_desc_size);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DRenderer12::GetDepthStencilView() const {
	return m_dsv_heap->GetCPUDescriptorHandleForHeapStart();
}
