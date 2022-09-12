#pragma once

#include <directx/d3dx12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <memory>

#include "descriptor_allocation.h"

class AdapterData;
class ByteAddressBuffer;
class CommandQueue;
class CommandList;
class ConstantBuffer;
class ConstantBufferView;
class DescriptorAllocator;
class GUI;
class IndexBuffer;
class PipelineStateObject;
class GraphicsPipelineState;
class ComputePipelineState;
class RenderTarget;
class Resource;
class RootSignature;
class Scene;
class ShaderResourceView;
class StructuredBuffer;
class SwapChain;
class Texture;
class UnorderedAccessView;
class VertexBuffer;
class VertexShader;
class PixelShader;
class Shader;

class Device {
public:
	static void EnableDebugLayer();
	static void ReportLiveObjects();

	static std::shared_ptr<Device> Create(std::shared_ptr<AdapterData> adapter = nullptr);

	std::wstring GetDescription() const;
	DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1);
	UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;
	std::shared_ptr<SwapChain> CreateSwapChain(HWND hWnd, DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM);
	std::shared_ptr<GUI> CreateGUI(HWND hWnd, const RenderTarget& render_target);
	std::shared_ptr<ConstantBuffer> CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
	std::shared_ptr<ByteAddressBuffer> CreateByteAddressBuffer(size_t buffer_size);
	std::shared_ptr<ByteAddressBuffer> CreateByteAddressBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
	std::shared_ptr<StructuredBuffer> CreateStructuredBuffer(size_t num_elements, size_t element_size);
	std::shared_ptr<StructuredBuffer> CreateStructuredBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_elements, size_t element_size);
	std::shared_ptr<Texture> CreateTexture(const D3D12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE* clear_value = nullptr);
	std::shared_ptr<Texture> CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clear_value = nullptr);
	std::shared_ptr<IndexBuffer> CreateIndexBuffer(size_t numIndices, DXGI_FORMAT index_format);
	std::shared_ptr<IndexBuffer> CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_indices, DXGI_FORMAT index_format);
	std::shared_ptr<VertexBuffer> CreateVertexBuffer(size_t num_vertices, size_t vertex_stride);
	std::shared_ptr<VertexBuffer> CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_vertices, size_t vertex_stride);
	std::shared_ptr<RootSignature> CreateRootSignature(const std::string& name, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& root_signature_desc);

	std::shared_ptr<GraphicsPipelineState> CreateGraphicsPipelineState(
		const std::string& name,
		std::shared_ptr<RootSignature> root_signature,
		std::shared_ptr<VertexShader> vertex_shader,
		std::shared_ptr<PixelShader> pixel_shader = nullptr,
		std::shared_ptr<Shader> domain_shader = nullptr,
		std::shared_ptr<Shader> hull_shader = nullptr,
		std::shared_ptr<Shader> geometry_shader = nullptr
	);

	std::shared_ptr<ComputePipelineState> CreateComputePipelineState(
		const std::string& name,
		std::shared_ptr<RootSignature> root_signature,
		std::shared_ptr<Shader> compute_shader
	);

	std::shared_ptr<ConstantBufferView> CreateConstantBufferView(const std::shared_ptr<ConstantBuffer>& constant_buffer, size_t offset = 0);
	std::shared_ptr<ShaderResourceView> CreateShaderResourceView(const std::shared_ptr<Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr);
	std::shared_ptr<UnorderedAccessView> CreateUnorderedAccessView(const std::shared_ptr<Resource>& resource, const std::shared_ptr<Resource>& counter_resource = nullptr, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav = nullptr);

	void Flush();
	void ReleaseStaleDescriptors();

	std::shared_ptr<AdapterData> GetAdapter() const;
	CommandQueue& GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

	Microsoft::WRL::ComPtr<ID3D12Device5> GetD3D12Device() const;
	D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() const;

	DXGI_SAMPLE_DESC GetMultisampleQualityLevels(DXGI_FORMAT format, UINT num_samples = D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE) const;

	bool SupportsUniversalHeaps() const;
	uint64_t MinimumHeapSize() const;
	uint64_t MandatoryHeapAlignment() const;
	uint64_t NodeMask() const;

protected:
	explicit Device(std::shared_ptr<AdapterData> adapter);
	virtual ~Device();

private:
	Microsoft::WRL::ComPtr<ID3D12Device5> m_d3d12_device;

	std::shared_ptr<AdapterData> m_adapter;

	std::unique_ptr<CommandQueue> m_direct_command_queue;
	std::unique_ptr<CommandQueue> m_compute_command_queue;
	std::unique_ptr<CommandQueue> m_copy_command_queue;

	std::unique_ptr<DescriptorAllocator> m_descriptor_allocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	D3D_ROOT_SIGNATURE_VERSION m_highest_root_signature_version;

	bool m_supports_universal_heaps = false;
	uint64_t m_minimum_heap_size = 1u;
	uint64_t m_heap_alignment = 1u;
	uint64_t m_node_mask = 0u;
};