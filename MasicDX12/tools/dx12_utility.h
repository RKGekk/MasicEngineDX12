#pragma once

#include <chrono>
#include <string>

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

UINT CalcConstantBufferByteSize(UINT byteSize);

void EnableDebugLayer();
void SetDebugInfoQueue(Microsoft::WRL::ComPtr<ID3D12Device5> device);

Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool use_warp);

Microsoft::WRL::ComPtr<ID3D12Device5> CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);
Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE type);
Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hwnd, Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue, uint32_t width, uint32_t height, uint32_t bufferCount);
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors);
Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE type);
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator, D3D12_COMMAND_LIST_TYPE type);

bool CheckTearingSupport();

Microsoft::WRL::ComPtr<ID3D12Fence> CreateFence(Microsoft::WRL::ComPtr<ID3D12Device5> device);
HANDLE CreateEventHandle();
uint64_t Signal(Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmd_queue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fence_value);
void WaitForFenceValue(Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t fence_value, HANDLE fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
void Flush(Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmd_queue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fence_event);

void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> cmd_list, Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES before_state, D3D12_RESOURCE_STATES after_state);

void ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> cmd_list, D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clear_color);
void ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> cmd_list, D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth);

void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> cmd_list, ID3D12Resource** ppDst_resource, ID3D12Resource** ppIntermediate_resource, size_t num_elements, size_t element_size, const void* buffer_data, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);


Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);
Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::string& filename);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const void* init_data, UINT64 byte_size, Microsoft::WRL::ComPtr<ID3D12Resource>& upload_buffer);
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const void* init_data, UINT64 byte_size);

static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::string& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);