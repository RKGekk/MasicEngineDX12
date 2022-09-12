#include "dx12_utility.h"

#include <cassert>
#include <fstream>

#include <d3dcompiler.h>
#include <directx/d3dx12.h>

#include "com_exception.h"
#include "string_utility.h"

using namespace std::literals;

UINT CalcConstantBufferByteSize(UINT byteSize) {
	// Constant buffers must be a multiple of the minimum hardware
	// allocation size (usually 256 bytes).  So round up to nearest
	// multiple of 256.  We do this by adding 255 and then masking off
	// the lower 2 bytes which store all bits < 256.
	// Example: Suppose byteSize = 300.
	// (300 + 255) & ~255
	// 555 & ~255
	// 0x022B & ~0x00ff
	// 0x022B & 0xff00
	// 0x0200
	// 512
	return (byteSize + 255) & ~255;
}

void EnableDebugLayer() {
	Microsoft::WRL::ComPtr<ID3D12Debug1> debug_interface;
	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug_interface.GetAddressOf()));
	ThrowIfFailed(hr);
	debug_interface->EnableDebugLayer();
}

void SetDebugInfoQueue(Microsoft::WRL::ComPtr<ID3D12Device5> device) {
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> p_info_queue;
	HRESULT hr = device.As(&p_info_queue);
	if (SUCCEEDED(hr)) {
		p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		D3D12_MESSAGE_SEVERITY Severities[] = {
			D3D12_MESSAGE_SEVERITY_INFO
		};

		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // This warning occurs when a render target is cleared using a clear color that is not the optimized clear color specified during resource creation. If you want to clear a render target using an arbitrary clear color, you should disable this warning.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		NewFilter.DenyList.NumCategories = 0u;
		NewFilter.DenyList.pCategoryList = nullptr;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		hr = p_info_queue->PushStorageFilter(&NewFilter);
		ThrowIfFailed(hr);
	}
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool use_warp) {
	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory;
	UINT create_factory_flags = 0u;
#if defined(_DEBUG)
	create_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	HRESULT hr = CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
	ThrowIfFailed(hr);

	Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter1;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgi_adapter4;
	if (use_warp) {
		hr = dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(dxgi_adapter1.GetAddressOf()));
		ThrowIfFailed(hr);
		hr = dxgi_adapter1.As(&dxgi_adapter4);
		ThrowIfFailed(hr);
	}
	else {
		SIZE_T max_dedicated_video_memory = 0u;
		for (UINT i = 0u; dxgi_factory->EnumAdapters1(i, dxgi_adapter1.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i) {
			DXGI_ADAPTER_DESC1 dxgi_adapter_desc1;
			dxgi_adapter1->GetDesc1(&dxgi_adapter_desc1);
			bool is_hw = (dxgi_adapter_desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
			bool is_gt_mem = dxgi_adapter_desc1.DedicatedVideoMemory > max_dedicated_video_memory;
			hr = D3D12CreateDevice(dxgi_adapter1.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr);
			bool is_dx12_cap = SUCCEEDED(hr);
			if (is_hw && is_gt_mem && is_dx12_cap) {
				max_dedicated_video_memory = dxgi_adapter_desc1.DedicatedVideoMemory;
				hr = dxgi_adapter1.As(&dxgi_adapter4);
				ThrowIfFailed(hr);
			}
		}
	}
	return dxgi_adapter4;
}

Microsoft::WRL::ComPtr<ID3D12Device5> CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter) {
	Microsoft::WRL::ComPtr<ID3D12Device5> d3d12_device2;
	HRESULT hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(d3d12_device2.GetAddressOf()));

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> p_info_queue;
	hr = d3d12_device2.As(&p_info_queue);
	if (SUCCEEDED(hr)) {
		p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		D3D12_MESSAGE_SEVERITY Severities[] = {
			D3D12_MESSAGE_SEVERITY_INFO
		};

		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // This warning occurs when a render target is cleared using a clear color that is not the optimized clear color specified during resource creation. If you want to clear a render target using an arbitrary clear color, you should disable this warning.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		NewFilter.DenyList.NumCategories = 0u;
		NewFilter.DenyList.pCategoryList = nullptr;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		hr = p_info_queue->PushStorageFilter(&NewFilter);
		ThrowIfFailed(hr);
	}
#endif

	return d3d12_device2;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE type) {
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3d12_command_queue;

	D3D12_COMMAND_QUEUE_DESC command_queue_desc;
	command_queue_desc.Type = type;
	command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	command_queue_desc.NodeMask = 0u;

	HRESULT hr = device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(d3d12_command_queue.GetAddressOf()));
	ThrowIfFailed(hr);

	return d3d12_command_queue;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hwnd, Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue, uint32_t width, uint32_t height, uint32_t bufferCount) {
	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory4;

	UINT create_factory_flags = 0;
#if defined(_DEBUG)
	create_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT hr = CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(dxgi_factory4.GetAddressOf()));
	ThrowIfFailed(hr);

	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
	swap_chain_desc.Width = width;
	swap_chain_desc.Height = height;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.Stereo = FALSE;
	swap_chain_desc.SampleDesc.Count = 1u;
	swap_chain_desc.SampleDesc.Quality = 0u;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = bufferCount;
	swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swap_chain_desc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain1;
	hr = dxgi_factory4->CreateSwapChainForHwnd(command_queue.Get(), hwnd, &swap_chain_desc, nullptr, nullptr, dxgi_swap_chain1.GetAddressOf());
	ThrowIfFailed(hr);

	hr = dxgi_factory4->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	ThrowIfFailed(hr);

	Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgi_swap_chain4;
	hr = dxgi_swap_chain1.As(&dxgi_swap_chain4);
	ThrowIfFailed(hr);

	return dxgi_swap_chain4;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors) {
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> desc_heap;
	D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
	descriptor_heap_desc.NumDescriptors = num_descriptors;
	descriptor_heap_desc.NodeMask = 0u;
	descriptor_heap_desc.Type = type;
	descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(desc_heap.GetAddressOf()));
	ThrowIfFailed(hr);
	return desc_heap;
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE type) {
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator;
	HRESULT hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(command_allocator.GetAddressOf()));
	ThrowIfFailed(hr);
	return command_allocator;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator, D3D12_COMMAND_LIST_TYPE type) {
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list;
	HRESULT hr = device->CreateCommandList(0, type, command_allocator.Get(), nullptr, IID_PPV_ARGS(command_list.GetAddressOf()));
	ThrowIfFailed(hr);
	hr = command_list->Close();
	ThrowIfFailed(hr);
	return command_list;
}

bool CheckTearingSupport() {
	BOOL allowTearing = FALSE;

	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory4;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(dxgi_factory4.GetAddressOf()));
	if (SUCCEEDED(hr)) {
		Microsoft::WRL::ComPtr<IDXGIFactory5> dxgi_factory5;
		hr = dxgi_factory4.As(&dxgi_factory5);
		if (SUCCEEDED(hr)) {
			hr = dxgi_factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
			if (FAILED(hr)) {
				allowTearing = FALSE;
			}
		}
	}

	return allowTearing == TRUE;
}

Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename) {
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	HRESULT hr = D3DCreateBlob(size, blob.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create d3d blob.");

	fin.read((char*)blob->GetBufferPointer(), size);
	fin.close();

	return blob;
}

Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::string& filename) {
	return LoadBinary(s2w(filename));
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const void* init_data, UINT64 byte_size, Microsoft::WRL::ComPtr<ID3D12Resource>& upload_buffer) {
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> default_buffer;

	// Create the actual default buffer resource.
	D3D12_HEAP_PROPERTIES heap_properties_default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size);
	hr = device->CreateCommittedResource(&heap_properties_default, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(default_buffer.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create default committed resource.");

	// In order to copy CPU memory data into our default buffer, we need to create
	// an intermediate upload heap.
	D3D12_HEAP_PROPERTIES heap_properties_upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	hr = device->CreateCommittedResource(&heap_properties_upload, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(upload_buffer.GetAddressOf()));

	// Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = init_data;
	subResourceData.RowPitch = byte_size;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// Schedule to copy the data to the default buffer resource. At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap. Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.
	D3D12_RESOURCE_BARRIER resource_barrier_common_to_copy_dest = CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	cmd_list->ResourceBarrier(1, &resource_barrier_common_to_copy_dest);
	UpdateSubresources<1>(cmd_list, default_buffer.Get(), upload_buffer.Get(), 0, 0, 1, &subResourceData);

	D3D12_RESOURCE_BARRIER resource_barrier_copy_dest_to_generic_read = CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	cmd_list->ResourceBarrier(1, &resource_barrier_copy_dest_to_generic_read);

	// Note: uploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy has been executed.
	return default_buffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const void* init_data, UINT64 byte_size) {
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> default_buffer;

	// Create the actual default buffer resource.
	D3D12_HEAP_PROPERTIES heap_properties_default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size);
	hr = device->CreateCommittedResource(&heap_properties_default, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(default_buffer.GetAddressOf()));
	COM_ERROR_IF_FAILED(hr, "Failed to create default committed resource.");

	return default_buffer;
}

Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target) {
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	HRESULT hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr) OutputDebugStringA((char*)errors->GetBufferPointer());

	COM_ERROR_IF_FAILED(hr, "Failed to compile shader.");

	return byteCode;
}

Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::string& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target) {
	return CompileShader(s2w(filename), defines, entrypoint, target);
}

Microsoft::WRL::ComPtr<ID3D12Fence> CreateFence(Microsoft::WRL::ComPtr<ID3D12Device5> device) {
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HRESULT hr = device->CreateFence(0u, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	ThrowIfFailed(hr);
	return fence;
}

HANDLE CreateEventHandle() {
	HANDLE fence_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fence_event && "Failed to create fence event.");
	return fence_event;
}

uint64_t Signal(Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmd_queue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fence_value) {
	uint64_t fence_value_for_signal = ++fence_value;
	HRESULT hr = cmd_queue->Signal(fence.Get(), fence_value_for_signal);
	ThrowIfFailed(hr);
	return fence_value_for_signal;
}

void WaitForFenceValue(Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t fence_value, HANDLE fenceEvent, std::chrono::milliseconds duration) {
	if (fence->GetCompletedValue() < fence_value) {
		HRESULT hr = fence->SetEventOnCompletion(fence_value, fenceEvent);
		ThrowIfFailed(hr);
		WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
	}
}

void Flush(Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmd_queue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fence_event) {
	uint64_t fence_value_for_signal = Signal(cmd_queue, fence, fenceValue);
	WaitForFenceValue(fence, fence_value_for_signal, fence_event);
}

void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> cmd_list, Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES before_state, D3D12_RESOURCE_STATES after_state) {
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), before_state, after_state);
	cmd_list->ResourceBarrier(1, &barrier);
}

void ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> cmd_list, D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clear_color) {
	cmd_list->ClearRenderTargetView(rtv, clear_color, 0u, nullptr);
}

void ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> cmd_list, D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth) {
	cmd_list->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0u, 0u, nullptr);
}

void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> cmd_list, ID3D12Resource** ppDst_resource, ID3D12Resource** ppIntermediate_resource, size_t num_nlements, size_t element_size, const void* buffer_data, D3D12_RESOURCE_FLAGS flags) {
	size_t buffer_size = num_nlements * element_size;
	D3D12_HEAP_PROPERTIES default_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC default_heap_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags);
	HRESULT hr = device->CreateCommittedResource(&default_heap_properties, D3D12_HEAP_FLAG_NONE, &default_heap_resource_desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(ppDst_resource));
	ThrowIfFailed(hr);

	if (buffer_data) {
		D3D12_HEAP_PROPERTIES upload_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC upload_heap_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
		hr = device->CreateCommittedResource(&upload_heap_properties, D3D12_HEAP_FLAG_NONE, &upload_heap_resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(ppIntermediate_resource));
		ThrowIfFailed(hr);

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = buffer_data;
		subresourceData.RowPitch = buffer_size;
		subresourceData.SlicePitch = subresourceData.RowPitch;

		UpdateSubresources(cmd_list.Get(), *ppDst_resource, *ppIntermediate_resource, 0ul, 0u, 1u, &subresourceData);
	}
}