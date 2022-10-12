#pragma once

#include "../vertex_types.h"
#include "../tools/defines.h"

#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

class Buffer;
class ByteAddressBuffer;
class ConstantBuffer;
class ConstantBufferView;
class Device;
class DynamicDescriptorHeap;
class GenerateMipsPSO;
class IndexBuffer;
class PanoToCubemapPSO;
class PipelineStateObject;
class RenderTarget;
class Resource;
class ResourceStateTracker;
class RootSignature;
class ShaderResourceView;
class StructuredBuffer;
class Texture;
class UnorderedAccessView;
class UploadBuffer;
class VertexBuffer;

class CommandList : public std::enable_shared_from_this<CommandList> {
public:

	D3D12_COMMAND_LIST_TYPE GetCommandListType() const;
	Device& GetDevice() const;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetD3D12CommandList() const;

	void TransitionBarrier(const std::shared_ptr<Resource>& resource, D3D12_RESOURCE_STATES state_after, UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flush_barriers = false);
	void TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES state_after, UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flush_barriers = false);

	void UAVBarrier(const std::shared_ptr<Resource>& resource = nullptr, bool flush_barriers = false);
	void UAVBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> resource, bool flush_barriers = false);

	void AliasingBarrier(const std::shared_ptr<Resource> & = nullptr, const std::shared_ptr<Resource>& after_resource = nullptr, bool flush_barriers = false);
	void AliasingBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> before_resource, Microsoft::WRL::ComPtr<ID3D12Resource> after_resource, bool flush_barriers = false);

	void FlushResourceBarriers();

	void CopyResource(const std::shared_ptr<Resource>& dst_res, const std::shared_ptr<Resource>& src_res);
	void CopyResource(Microsoft::WRL::ComPtr<ID3D12Resource> dst_res, Microsoft::WRL::ComPtr<ID3D12Resource> src_res);

	void ResolveSubresource(const std::shared_ptr<Resource>&, const std::shared_ptr<Resource>&, uint32_t dst_subresource = 0, uint32_t src_subresource = 0);

	std::shared_ptr<VertexBuffer> CopyVertexBuffer(size_t num_vertices, size_t vertex_stride, const void* vertex_buffer_data);

	template<typename T>
	std::shared_ptr<VertexBuffer> CopyVertexBuffer(const std::vector<T>& vertex_buffer_data) {
		return CopyVertexBuffer(vertex_buffer_data.size(), sizeof(T), vertex_buffer_data.data());
	}

	std::shared_ptr<IndexBuffer> CopyIndexBuffer(size_t num_indices, DXGI_FORMAT index_format, const void* index_buffer_data);

	template<typename T>
	std::shared_ptr<IndexBuffer> CopyIndexBuffer(const std::vector<T>& index_buffer_data) {
		assert(sizeof(T) == 2u || sizeof(T) == 4u);

		DXGI_FORMAT index_format = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		return CopyIndexBuffer(index_buffer_data.size(), index_format, index_buffer_data.data());
	}

	std::shared_ptr<ConstantBuffer> CopyConstantBuffer(size_t buffer_size, const void* buffer_data);

	template<typename T>
	std::shared_ptr<ConstantBuffer> CopyConstantBuffer(const T& data) {
		return CopyConstantBuffer(sizeof(T), &data);
	}

	std::shared_ptr<ByteAddressBuffer> CopyByteAddressBuffer(size_t buffer_size, const void* buffer_data);

	template<typename T>
	std::shared_ptr<ByteAddressBuffer> CopyByteAddressBuffer(const T& data) {
		return CopyByteAddressBuffer(sizeof(T), &data);
	}

	std::shared_ptr<StructuredBuffer> CopyStructuredBuffer(size_t num_elements, size_t element_size, const void* buffer_data, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	template<typename T>
	std::shared_ptr<StructuredBuffer> CopyStructuredBuffer(const std::vector<T>& buffer_data) {
		return CopyStructuredBuffer(buffer_data.size(), sizeof(T), buffer_data.data());
	}

	template<typename T>
	std::shared_ptr<StructuredBuffer> CopyStructuredBuffer(const std::vector<T>& buffer_data, D3D12_RESOURCE_FLAGS flags) {
		return CopyStructuredBuffer(buffer_data.size(), sizeof(T), buffer_data.data(), flags);
	}

	void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitive_topology);

	std::shared_ptr<Texture> LoadTextureFromFile(const std::wstring& file_name, bool sRGB = false);

	void ClearTexture(const std::shared_ptr<Texture>& texture, const float clear_color[4]);
	void ClearDepthStencilTexture(const std::shared_ptr<Texture>& texture, D3D12_CLEAR_FLAGS clear_flags, float depth = 1.0f, uint8_t stencil = 0);

	void GenerateMips(const std::shared_ptr<Texture>& texture);

	void PanoToCubemap(const std::shared_ptr<Texture>& cubemap_texture, const std::shared_ptr<Texture>& pano_texture);

	void CopyTextureSubresource(const std::shared_ptr<Texture>& texture, uint32_t first_subresource, uint32_t num_subresources, D3D12_SUBRESOURCE_DATA* subresource_data);

	void SetGraphicsDynamicConstantBuffer(uint32_t root_parameter_index, size_t size_in_bytes, const void* buffer_data);

	template<typename T>
	void SetGraphicsDynamicConstantBuffer(uint32_t root_parameter_index, const T& data) {
		SetGraphicsDynamicConstantBuffer(root_parameter_index, sizeof(T), &data);
	}

	void SetGraphics32BitConstants(uint32_t root_parameter_index, uint32_t num_constants, const void* constants);

	template<typename T>
	void SetGraphics32BitConstants(uint32_t root_parameter_index, const T& constants) {
		static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
		SetGraphics32BitConstants(root_parameter_index, sizeof(T) / sizeof(uint32_t), &constants);
	}

	void SetCompute32BitConstants(uint32_t root_parameter_index, uint32_t num_constants, const void* constants);

	template<typename T>
	void SetCompute32BitConstants(uint32_t root_parameter_index, const T& constants) {
		static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
		SetCompute32BitConstants(root_parameter_index, sizeof(T) / sizeof(uint32_t), &constants);
	}

	void SetVertexBuffers(uint32_t start_slot, const std::vector<std::shared_ptr<VertexBuffer>>& vertex_buffer_views);
	void SetVertexBuffer(uint32_t slot, const std::shared_ptr<VertexBuffer>& vertex_buffer_view);

	void SetDynamicVertexBuffer(uint32_t slot, size_t num_vertices, size_t vertex_size, const void* vertex_buffer_data);

	template<typename T>
	void SetDynamicVertexBuffer(uint32_t slot, const std::vector<T>& vertex_buffer_data) {
		SetDynamicVertexBuffer(slot, vertex_buffer_data.size(), sizeof(T), vertex_buffer_data.data());
	}

	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer);

	void SetDynamicIndexBuffer(size_t num_indices, DXGI_FORMAT index_format, const void* index_buffer_data);

	template<typename T>
	void SetDynamicIndexBuffer(const std::vector<T>& index_buffer_data) {
		static_assert(sizeof(T) == 2u || sizeof(T) == 4u);

		DXGI_FORMAT index_format = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		SetDynamicIndexBuffer(index_buffer_data.size(), index_format, index_buffer_data.data());
	}

	void SetGraphicsDynamicStructuredBuffer(uint32_t slot, size_t num_elements, size_t element_size, const void* buffer_data);

	template<typename T>
	void SetGraphicsDynamicStructuredBuffer(uint32_t slot, const std::vector<T>& buffer_data) {
		SetGraphicsDynamicStructuredBuffer(slot, buffer_data.size(), sizeof(T), buffer_data.data());
	}

	void SetViewport(const D3D12_VIEWPORT& viewport);
	void SetViewports(const std::vector<D3D12_VIEWPORT>& viewports);

	void SetScissorRect(const D3D12_RECT& scissorRect);
	void SetScissorRects(const std::vector<D3D12_RECT>& scissor_rects);

	void SetPipelineState(const std::shared_ptr<PipelineStateObject>& pipeline_state);

	void SetGraphicsRootSignature(const std::shared_ptr<RootSignature>& root_signature);
	void SetComputeRootSignature(const std::shared_ptr<RootSignature>& root_signature);

	void SetConstantBufferView(uint32_t root_parameter_index, const std::shared_ptr<ConstantBuffer>& buffer, D3D12_RESOURCE_STATES state_after = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, size_t buffer_offset = 0);

	void SetShaderResourceView(uint32_t root_parameter_index, const std::shared_ptr<Buffer>& buffer, D3D12_RESOURCE_STATES state_after = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, size_t buffer_offset = 0);
	void SetUnorderedAccessView(uint32_t root_parameter_index, const std::shared_ptr<Buffer>& buffer, D3D12_RESOURCE_STATES state_after = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, size_t buffer_offset = 0);
	void SetConstantBufferView(uint32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<ConstantBufferView>& cbv, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	void SetShaderResourceView(uint32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<ShaderResourceView>& srv, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, UINT first_subresource = 0, UINT num_subresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
	void SetShaderResourceView(int32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<Texture>& texture, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, UINT first_subresource = 0, UINT num_subresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
	void SetUnorderedAccessView(uint32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<UnorderedAccessView>& uav, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, UINT first_subresource = 0, UINT num_subresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
	void SetUnorderedAccessView(uint32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<Texture>& texture, UINT mip, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, UINT first_subresource = 0, UINT num_subresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

	void SetRenderTarget(const RenderTarget& render_target);

	void Draw(uint32_t vertex_count, uint32_t instance_count = 1u, uint32_t start_vertex = 0u, uint32_t start_instance = 0u);
	void DrawIndexed(uint32_t index_count, uint32_t instance_count = 1u, uint32_t start_index = 0u, int32_t base_vertex = 0u, uint32_t startInstance = 0u);
	void Dispatch(uint32_t num_groups_x, uint32_t num_groups_y = 1u, uint32_t num_groups_z = 1u);

protected:
	friend class CommandQueue;
	friend class DynamicDescriptorHeap;

	CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CommandList();

	bool Close(const std::shared_ptr<CommandList>& pending_command_list);
	void Close();
	void Reset();
	void ReleaseTrackedObjects();

	void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, ID3D12DescriptorHeap* heap);

	std::shared_ptr<CommandList> GetGenerateMipsCommandList() const;

private:
	using VertexCollection = std::vector<VertexPositionNormalTangentBitangentTexture>;
	using IndexCollection = std::vector<uint16_t>;

	inline void ReverseWinding(IndexCollection& indices, VertexCollection& vertices);
	inline void InvertNormals(VertexCollection& vertices);
	inline DirectX::XMVECTOR GetCircleVector(size_t i, size_t tessellation) noexcept;
	inline DirectX::XMVECTOR GetCircleTangent(size_t i, size_t tessellation) noexcept;
	void CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation, float height, float radius, bool is_top);

	void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object);
	void TrackResource(const std::shared_ptr<Resource>& res);

	void GenerateMips_UAV(const std::shared_ptr<Texture>& texture, bool is_sRGB);

	Microsoft::WRL::ComPtr<ID3D12Resource> CopyBuffer(size_t buffer_size, const void* buffer_data, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	void BindDescriptorHeaps();

	Device& m_device;
	D3D12_COMMAND_LIST_TYPE m_d3d12_command_list_type;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_d3d12_command_list;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_d3d12_command_allocator;

	std::shared_ptr<CommandList> m_compute_command_list;

	ID3D12RootSignature* m_root_signature;
	ID3D12PipelineState* m_pipeline_state;

	std::unique_ptr<UploadBuffer> m_upload_buffer;
	std::unique_ptr<ResourceStateTracker> m_resource_state_tracker;
	std::unique_ptr<DynamicDescriptorHeap> m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	ID3D12DescriptorHeap* m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	std::unique_ptr<GenerateMipsPSO> m_generate_mips_pso;
	std::unique_ptr<PanoToCubemapPSO> m_pano_to_cubemap_pso;

	using TrackedObjects = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;

	TrackedObjects m_tracked_objects;

	static std::map<std::wstring, ID3D12Resource*> ms_texture_cache;
	static std::mutex ms_texture_cache_mutex;
};

inline DirectX::XMVECTOR CommandList::GetCircleVector(size_t i, size_t tessellation) noexcept {
	float angle = float(i) * DirectX::XM_2PI / float(tessellation);
	float dx, dz;

	DirectX::XMScalarSinCos(&dx, &dz, angle);

	DirectX::XMVECTORF32 v = { { { dx, 0, dz, 0 } } };
	return v;
}

inline DirectX::XMVECTOR CommandList::GetCircleTangent(size_t i, size_t tessellation) noexcept {
	float angle = (float(i) * DirectX::XM_2PI / float(tessellation)) + DirectX::XM_PIDIV2;
	float dx, dz;

	DirectX::XMScalarSinCos(&dx, &dz, angle);

	DirectX::XMVECTORF32 v = { { { dx, 0, dz, 0 } } };
	return v;
}

inline void CommandList::CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation, float height, float radius, bool is_top) {
	// Create cap indices.
	for (size_t i = 0; i < tessellation - 2; i++) {
		size_t i1 = (i + 1) % tessellation;
		size_t i2 = (i + 2) % tessellation;

		if (is_top) {
			std::swap(i1, i2);
		}

		size_t vbase = vertices.size();
		indices.push_back(static_cast<IndexCollection::value_type>(vbase + i2));
		indices.push_back(static_cast<IndexCollection::value_type>(vbase + i1));
		indices.push_back(static_cast<IndexCollection::value_type>(vbase));
	}

	// Which end of the cylinder is this?
	DirectX::XMVECTOR normal = DirectX::g_XMIdentityR1;
	DirectX::XMVECTOR texture_scale = DirectX::g_XMNegativeOneHalf;

	if (!is_top) {
		normal = DirectX::XMVectorNegate(normal);
		texture_scale = DirectX::XMVectorMultiply(texture_scale, DirectX::g_XMNegateX);
	}

	// Create cap vertices.
	for (size_t i = 0; i < tessellation; i++) {
		DirectX::XMVECTOR circle_vector = GetCircleVector(i, tessellation);
		DirectX::XMVECTOR position = DirectX::XMVectorAdd(DirectX::XMVectorScale(circle_vector, radius), DirectX::XMVectorScale(normal, height));
		DirectX::XMVECTOR texture_coordinate = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorSwizzle<0, 2, 3, 3>(circle_vector), texture_scale, DirectX::g_XMOneHalf);

		vertices.emplace_back(position, normal, texture_coordinate);
	}
}

inline void CommandList::ReverseWinding(IndexCollection& indices, VertexCollection& vertices) {
	assert((indices.size() % 3) == 0);
	for (auto it = indices.begin(); it != indices.end(); it += 3) {
		std::swap(*it, *(it + 2));
	}

	for (auto it = vertices.begin(); it != vertices.end(); ++it) {
		it->TexCoord.x = (1.f - it->TexCoord.x);
	}
}

inline void CommandList::InvertNormals(VertexCollection& vertices) {
	for (auto it = vertices.begin(); it != vertices.end(); ++it) {
		it->Normal.x = -it->Normal.x;
		it->Normal.y = -it->Normal.y;
		it->Normal.z = -it->Normal.z;
	}
}