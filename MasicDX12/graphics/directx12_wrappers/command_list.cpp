#include "command_list.h"

#include "byte_address_buffer.h"
#include "command_queue.h"
#include "constant_buffer.h"
#include "constant_buffer_view.h"
#include "device.h"
#include "dynamic_descriptor_heap.h"
#include "../generate_mips_pso.h"
#include "index_buffer.h"
#include "../material.h"
#include "../mesh.h"
#include "../pano_to_cubemap_pso.h"
#include "pipeline_state_object.h"
#include "render_target.h"
#include "resource.h"
#include "resource_state_tracker.h"
#include "root_signature.h"
#include "shader_resource_view.h"
#include "structured_buffer.h"
#include "texture.h"
#include "unordered_access_view.h"
#include "upload_buffer.h"
#include "vertex_buffer.h"
#include "../tools/com_exception.h"
#include "../tools/math_utitity.h"
#include "../tools/memory_utility.h"

#include <filesystem>

#include <DirectXTex/DirectXTex.h>

class MakeUploadBuffer : public UploadBuffer {
public:
	MakeUploadBuffer(Device& device, size_t pageSize = _2MB)
		: UploadBuffer(device, pageSize) {}

	virtual ~MakeUploadBuffer() {}
};

std::map<std::wstring, ID3D12Resource*> CommandList::ms_texture_cache;
std::mutex CommandList::ms_texture_cache_mutex;

D3D12_COMMAND_LIST_TYPE CommandList::GetCommandListType() const {
	return m_d3d12_command_list_type;
}

Device& CommandList::GetDevice() const {
	return m_device;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandList::GetD3D12CommandList() const {
	return m_d3d12_command_list;
}

std::shared_ptr<CommandList> CommandList::GetGenerateMipsCommandList() const {
	return m_compute_command_list;
}

CommandList::CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type) : m_device(device), m_d3d12_command_list_type(type), m_root_signature(nullptr), m_pipeline_state(nullptr) {
	auto d3d12_device = m_device.GetD3D12Device();

	HRESULT hr = d3d12_device->CreateCommandAllocator(m_d3d12_command_list_type, IID_PPV_ARGS(m_d3d12_command_allocator.GetAddressOf()));
	ThrowIfFailed(hr);

	hr = d3d12_device->CreateCommandList(0, m_d3d12_command_list_type, m_d3d12_command_allocator.Get(), nullptr, IID_PPV_ARGS(m_d3d12_command_list.GetAddressOf()));
	ThrowIfFailed(hr);

	m_upload_buffer = std::make_unique<MakeUploadBuffer>(device);
	m_resource_state_tracker = std::make_unique<ResourceStateTracker>();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamic_descriptor_heap[i] = std::make_unique<DynamicDescriptorHeap>(device, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
		m_descriptor_heaps[i] = nullptr;
	}
}

CommandList::~CommandList() {}

void CommandList::TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES state_after, UINT subresource, bool flush_barriers) {
	if (resource) {
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON, state_after, subresource);
		m_resource_state_tracker->ResourceBarrier(barrier);
	}

	if (flush_barriers) {
		FlushResourceBarriers();
	}
}

void CommandList::TransitionBarrier(const std::shared_ptr<Resource>& resource, D3D12_RESOURCE_STATES state_after, UINT subresource, bool flush_barriers) {
	if (resource) {
		TransitionBarrier(resource->GetD3D12Resource(), state_after, subresource, flush_barriers);
	}
}

void CommandList::UAVBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> resource, bool flush_barriers) {
	auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource.Get());

	m_resource_state_tracker->ResourceBarrier(barrier);

	if (flush_barriers) {
		FlushResourceBarriers();
	}
}

void CommandList::UAVBarrier(const std::shared_ptr<Resource>& resource, bool flush_barriers) {
	auto d3d12_resource = resource ? resource->GetD3D12Resource() : nullptr;
	UAVBarrier(d3d12_resource, flush_barriers);
}

void CommandList::AliasingBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> before_resource, Microsoft::WRL::ComPtr<ID3D12Resource> after_resource, bool flush_barriers) {
	auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(before_resource.Get(), after_resource.Get());

	m_resource_state_tracker->ResourceBarrier(barrier);

	if (flush_barriers) {
		FlushResourceBarriers();
	}
}

void CommandList::AliasingBarrier(const std::shared_ptr<Resource>& before_resource, const std::shared_ptr<Resource>& after_resource, bool flush_barriers) {
	auto d3d12_before_resource = before_resource ? before_resource->GetD3D12Resource() : nullptr;
	auto d3d12_after_resource = after_resource ? after_resource->GetD3D12Resource() : nullptr;

	AliasingBarrier(d3d12_before_resource, d3d12_after_resource, flush_barriers);
}

void CommandList::FlushResourceBarriers() {
	m_resource_state_tracker->FlushResourceBarriers(shared_from_this());
}

void CommandList::CopyResource(Microsoft::WRL::ComPtr<ID3D12Resource> dst_res, Microsoft::WRL::ComPtr<ID3D12Resource> src_res) {
	assert(dst_res);
	assert(src_res);

	TransitionBarrier(dst_res, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(src_res, D3D12_RESOURCE_STATE_COPY_SOURCE);

	FlushResourceBarriers();

	m_d3d12_command_list->CopyResource(dst_res.Get(), src_res.Get());

	TrackResource(dst_res);
	TrackResource(src_res);
}

void CommandList::CopyResource(const std::shared_ptr<Resource>& dst_res, const std::shared_ptr<Resource>& src_res) {
	assert(dst_res && src_res);

	CopyResource(dst_res->GetD3D12Resource(), src_res->GetD3D12Resource());
}

void CommandList::ResolveSubresource(const std::shared_ptr<Resource>& dst_res, const std::shared_ptr<Resource>& src_res, uint32_t dst_subresource, uint32_t src_subresource) {
	assert(dst_res && src_res);

	TransitionBarrier(dst_res, D3D12_RESOURCE_STATE_RESOLVE_DEST, dst_subresource);
	TransitionBarrier(src_res, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, src_subresource);

	FlushResourceBarriers();

	m_d3d12_command_list->ResolveSubresource(dst_res->GetD3D12Resource().Get(), dst_subresource, src_res->GetD3D12Resource().Get(), src_subresource, dst_res->GetD3D12ResourceDesc().Format);

	TrackResource(src_res);
	TrackResource(dst_res);
}

Microsoft::WRL::ComPtr<ID3D12Resource> CommandList::CopyBuffer(size_t buffer_size, const void* buffer_data, D3D12_RESOURCE_FLAGS flags) {
	Microsoft::WRL::ComPtr<ID3D12Resource> d3d12_resource;
	if (buffer_size == 0u) return d3d12_resource;

	auto d3d12_device = m_device.GetD3D12Device();

	D3D12_HEAP_PROPERTIES heap_props_def = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC def_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags);
	HRESULT hr = d3d12_device->CreateCommittedResource(&heap_props_def, D3D12_HEAP_FLAG_NONE, &def_resource_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(d3d12_resource.GetAddressOf()));
	ThrowIfFailed(hr);

	ResourceStateTracker::AddGlobalResourceState(d3d12_resource.Get(), D3D12_RESOURCE_STATE_COMMON);

	if (buffer_data != nullptr) {
		Microsoft::WRL::ComPtr<ID3D12Resource> upload_resource;
		D3D12_HEAP_PROPERTIES heap_props_upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC upload_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
		hr = d3d12_device->CreateCommittedResource(&heap_props_upload, D3D12_HEAP_FLAG_NONE, &upload_resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(upload_resource.GetAddressOf()));
		ThrowIfFailed(hr);

		D3D12_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pData = buffer_data;
		subresource_data.RowPitch = buffer_size;
		subresource_data.SlicePitch = subresource_data.RowPitch;

		m_resource_state_tracker->TransitionResource(d3d12_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		UpdateSubresources(m_d3d12_command_list.Get(), d3d12_resource.Get(), upload_resource.Get(), 0ul, 0u, 1u, &subresource_data);

		TrackResource(upload_resource);
	}
	TrackResource(d3d12_resource);

	return d3d12_resource;
}

std::shared_ptr<VertexBuffer> CommandList::CopyVertexBuffer(size_t num_vertices, size_t vertex_stride, const void* vertex_buffer_data) {
	auto d3d12_resource = CopyBuffer(num_vertices * vertex_stride, vertex_buffer_data);
	std::shared_ptr<VertexBuffer> vertex_buffer = m_device.CreateVertexBuffer(d3d12_resource, num_vertices, vertex_stride);

	return vertex_buffer;
}

std::shared_ptr<IndexBuffer> CommandList::CopyIndexBuffer(size_t num_indices, DXGI_FORMAT index_format, const void* index_buffer_data) {
	size_t element_size = index_format == DXGI_FORMAT_R16_UINT ? 2u : 4u;
	auto d3d12_resource = CopyBuffer(num_indices * element_size, index_buffer_data);
	std::shared_ptr<IndexBuffer> index_buffer = m_device.CreateIndexBuffer(d3d12_resource, num_indices, index_format);

	return index_buffer;
}

std::shared_ptr<ConstantBuffer> CommandList::CopyConstantBuffer(size_t buffer_size, const void* buffer_data) {
	auto d3d12_resource = CopyBuffer(buffer_size, buffer_data);
	std::shared_ptr<ConstantBuffer> constant_buffer = m_device.CreateConstantBuffer(d3d12_resource);

	return constant_buffer;
}

std::shared_ptr<ByteAddressBuffer> CommandList::CopyByteAddressBuffer(size_t buffer_size, const void* buffer_data) {
	auto d3d12_resource = CopyBuffer(buffer_size, buffer_data, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	std::shared_ptr<ByteAddressBuffer> byte_address_buffer = m_device.CreateByteAddressBuffer(d3d12_resource);

	return byte_address_buffer;
}

std::shared_ptr<StructuredBuffer> CommandList::CopyStructuredBuffer(size_t num_elements, size_t element_size, const void* buffer_data, D3D12_RESOURCE_FLAGS flags) {
	auto d3d12_resource = CopyBuffer(num_elements * element_size, buffer_data, flags);
	std::shared_ptr<StructuredBuffer> structured_buffer = m_device.CreateStructuredBuffer(d3d12_resource, num_elements, element_size);

	return structured_buffer;
}

void CommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitive_topology) {
	m_d3d12_command_list->IASetPrimitiveTopology(primitive_topology);
}

std::shared_ptr<Texture> CommandList::LoadTextureFromFile(const std::wstring& file_name, bool sRGB) {
	using namespace DirectX;
	std::shared_ptr<Texture> texture;
	std::filesystem::path file_path(file_name);
	if (!std::filesystem::exists(file_path)) {
		throw std::exception("File not found.");
	}

	std::lock_guard<std::mutex> lock(ms_texture_cache_mutex);
	auto iter = ms_texture_cache.find(file_name);
	if (iter != ms_texture_cache.end()) {
		texture = m_device.CreateTexture(iter->second);
	}
	else {
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage scratch_image;

		if (file_path.extension() == ".dds") {
			ThrowIfFailed(LoadFromDDSFile(file_name.c_str(), DirectX::DDS_FLAGS_FORCE_RGB, &metadata, scratch_image));
		}
		else if (file_path.extension() == ".hdr") {
			ThrowIfFailed(LoadFromHDRFile(file_name.c_str(), &metadata, scratch_image));
		}
		else if (file_path.extension() == ".tga") {
			ThrowIfFailed(LoadFromTGAFile(file_name.c_str(), &metadata, scratch_image));
		}
		else {
			ThrowIfFailed(LoadFromWICFile(file_name.c_str(), DirectX::WIC_FLAGS_FORCE_RGB | DirectX::WIC_FLAGS_IGNORE_SRGB, &metadata, scratch_image));
		}

		if (sRGB) {
			metadata.format = DirectX::MakeSRGB(metadata.format);
		}

		D3D12_RESOURCE_DESC texture_desc = {};
		switch (metadata.dimension) {
			case DirectX::TEX_DIMENSION_TEXTURE1D:
				texture_desc = CD3DX12_RESOURCE_DESC::Tex1D(metadata.format, static_cast<UINT64>(metadata.width), static_cast<UINT16>(metadata.arraySize));
				break;
			case DirectX::TEX_DIMENSION_TEXTURE2D:
				texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, static_cast<UINT64>(metadata.width), static_cast<UINT>(metadata.height), static_cast<UINT16>(metadata.arraySize));
				break;
			case DirectX::TEX_DIMENSION_TEXTURE3D:
				texture_desc = CD3DX12_RESOURCE_DESC::Tex3D(metadata.format, static_cast<UINT64>(metadata.width), static_cast<UINT>(metadata.height), static_cast<UINT16>(metadata.depth));
				break;
			default:
				throw std::exception("Invalid texture dimension.");
				break;
		}

		auto d3d12_device = m_device.GetD3D12Device();
		Microsoft::WRL::ComPtr<ID3D12Resource> texture_resource;


		D3D12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HRESULT hr = d3d12_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &texture_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(texture_resource.GetAddressOf()));
		ThrowIfFailed(hr);

		texture = m_device.CreateTexture(texture_resource);
		texture->SetName(file_name);

		ResourceStateTracker::AddGlobalResourceState(texture_resource.Get(), D3D12_RESOURCE_STATE_COMMON);

		std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratch_image.GetImageCount());
		const DirectX::Image* pImages = scratch_image.GetImages();
		for (int i = 0; i < scratch_image.GetImageCount(); ++i) {
			auto& subresource = subresources[i];
			subresource.RowPitch = pImages[i].rowPitch;
			subresource.SlicePitch = pImages[i].slicePitch;
			subresource.pData = pImages[i].pixels;
		}

		CopyTextureSubresource(texture, 0, static_cast<uint32_t>(subresources.size()), subresources.data());

		if (subresources.size() < texture_resource->GetDesc().MipLevels) {
			GenerateMips(texture);
		}

		ms_texture_cache[file_name] = texture_resource.Get();
	}

	return texture;
}

void CommandList::GenerateMips(const std::shared_ptr<Texture>& texture) {
	if (!texture)
		return;

	auto d3d12_device = m_device.GetD3D12Device();

	if (m_d3d12_command_list_type == D3D12_COMMAND_LIST_TYPE_COPY) {
		if (!m_compute_command_list) {
			m_compute_command_list = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE).GetCommandList();
		}
		m_compute_command_list->GenerateMips(texture);
		return;
	}

	auto d3d12_resource = texture->GetD3D12Resource();

	if (!d3d12_resource) return;

	auto resource_desc = d3d12_resource->GetDesc();

	if (resource_desc.MipLevels == 1) return;

	if (resource_desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || resource_desc.DepthOrArraySize != 1 || resource_desc.SampleDesc.Count > 1) {
		throw std::exception("GenerateMips is only supported for non-multi-sampled 2D Textures.");
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> uav_resource = d3d12_resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> alias_resource;

	if (!texture->CheckUAVSupport() || (resource_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0) {
		auto alias_desc = resource_desc;

		alias_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		alias_desc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		auto uav_desc = alias_desc;
		uav_desc.Format = Texture::GetUAVCompatableFormat(resource_desc.Format);

		D3D12_RESOURCE_DESC resource_descs[] = { alias_desc, uav_desc };

		auto allocationInfo = d3d12_device->GetResourceAllocationInfo(0, _countof(resource_descs), resource_descs);

		D3D12_HEAP_DESC heap_desc = {};
		heap_desc.SizeInBytes = allocationInfo.SizeInBytes;
		heap_desc.Alignment = allocationInfo.Alignment;
		heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
		heap_desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

		Microsoft::WRL::ComPtr<ID3D12Heap> heap;
		HRESULT hr = d3d12_device->CreateHeap(&heap_desc, IID_PPV_ARGS(heap.GetAddressOf()));
		ThrowIfFailed(hr);

		TrackResource(heap);

		hr = d3d12_device->CreatePlacedResource(heap.Get(), 0, &alias_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(alias_resource.GetAddressOf()));
		ThrowIfFailed(hr);

		ResourceStateTracker::AddGlobalResourceState(alias_resource.Get(), D3D12_RESOURCE_STATE_COMMON);

		TrackResource(alias_resource);

		hr = d3d12_device->CreatePlacedResource(heap.Get(), 0, &uav_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(uav_resource.ReleaseAndGetAddressOf()));
		ThrowIfFailed(hr);

		ResourceStateTracker::AddGlobalResourceState(uav_resource.Get(), D3D12_RESOURCE_STATE_COMMON);

		TrackResource(uav_resource);
		AliasingBarrier(nullptr, alias_resource);

		CopyResource(alias_resource, d3d12_resource);
		AliasingBarrier(alias_resource, uav_resource);
	}

	auto uav_texture = m_device.CreateTexture(uav_resource);
	GenerateMips_UAV(uav_texture, Texture::IsSRGBFormat(resource_desc.Format));

	if (alias_resource) {
		AliasingBarrier(uav_resource, alias_resource);
		CopyResource(d3d12_resource, alias_resource);
	}
}

void CommandList::GenerateMips_UAV(const std::shared_ptr<Texture>& texture, bool is_sRGB) {
	if (!m_generate_mips_pso) {
		m_generate_mips_pso = std::make_unique<GenerateMipsPSO>(m_device);
	}

	SetPipelineState(m_generate_mips_pso->GetPipelineState());
	SetComputeRootSignature(m_generate_mips_pso->GetRootSignature());

	GenerateMipsCB generate_mips_cb;
	generate_mips_cb.IsSRGB = is_sRGB;

	auto resource = texture->GetD3D12Resource();
	auto resource_desc = resource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = is_sRGB ? Texture::GetSRGBFormat(resource_desc.Format) : resource_desc.Format;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = resource_desc.MipLevels;

	auto srv = m_device.CreateShaderResourceView(texture, &srv_desc);

	for (uint32_t src_mip = 0; src_mip < resource_desc.MipLevels - 1u; ) {

		uint64_t src_width = resource_desc.Width >> src_mip;
		uint32_t src_height = resource_desc.Height >> src_mip;
		uint32_t dst_width = static_cast<uint32_t>(src_width >> 1);
		uint32_t dst_height = src_height >> 1;

		generate_mips_cb.SrcDimension = (src_height & 1) << 1 | (src_width & 1);

		DWORD mip_count;
		_BitScanForward(&mip_count, (dst_width == 1 ? dst_height : dst_width) | (dst_height == 1 ? dst_width : dst_height));

		mip_count = std::min<DWORD>(4, mip_count + 1);
		mip_count = (src_mip + mip_count) >= resource_desc.MipLevels ? resource_desc.MipLevels - src_mip - 1 : mip_count;

		dst_width = std::max<DWORD>(1, dst_width);
		dst_height = std::max<DWORD>(1, dst_height);

		generate_mips_cb.SrcMipLevel = src_mip;
		generate_mips_cb.NumMipLevels = mip_count;
		generate_mips_cb.TexelSize.x = 1.0f / (float)dst_width;
		generate_mips_cb.TexelSize.y = 1.0f / (float)dst_height;

		SetCompute32BitConstants(GenerateMips::GenerateMipsCB, generate_mips_cb);

		SetShaderResourceView(GenerateMips::SrcMip, 0u, srv, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, src_mip, 1u);

		for (uint32_t mip = 0; mip < mip_count; ++mip) {
			D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
			uav_desc.Format = resource_desc.Format;
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uav_desc.Texture2D.MipSlice = src_mip + mip + 1;

			auto uav = m_device.CreateUnorderedAccessView(texture, nullptr, &uav_desc);
			SetUnorderedAccessView(GenerateMips::OutMip, mip, uav, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, src_mip + mip + 1u, 1u);
		}

		if (mip_count < 4) {
			m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(GenerateMips::OutMip, mip_count, 4 - mip_count, m_generate_mips_pso->GetDefaultUAV());
		}

		Dispatch(Math::DivideByMultiple(dst_width, 8u), Math::DivideByMultiple(dst_height, 8u));

		UAVBarrier(texture);

		src_mip += mip_count;
	}
}

void CommandList::PanoToCubemap(const std::shared_ptr<Texture>& cubemap_texture, const std::shared_ptr<Texture>& pano_texture) {
	assert(cubemap_texture && pano_texture);

	if (m_d3d12_command_list_type == D3D12_COMMAND_LIST_TYPE_COPY) {
		if (!m_compute_command_list) {
			m_compute_command_list = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE).GetCommandList();
		}
		m_compute_command_list->PanoToCubemap(cubemap_texture, pano_texture);
		return;
	}

	if (!m_pano_to_cubemap_pso) {
		m_pano_to_cubemap_pso = std::make_unique<PanoToCubemapPSO>(m_device);
	}

	auto cubemap_resource = cubemap_texture->GetD3D12Resource();
	if (!cubemap_resource) return;

	CD3DX12_RESOURCE_DESC cubemap_desc(cubemap_resource->GetDesc());

	auto staging_resource = cubemap_resource;
	auto staging_texture = m_device.CreateTexture(staging_resource);

	if ((cubemap_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0) {
		auto d3d12_device = m_device.GetD3D12Device();

		auto staging_desc = cubemap_desc;
		staging_desc.Format = Texture::GetUAVCompatableFormat(cubemap_desc.Format);
		staging_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;


		D3D12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HRESULT hr = d3d12_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &staging_desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(staging_resource.GetAddressOf()));
		ThrowIfFailed(hr);

		ResourceStateTracker::AddGlobalResourceState(staging_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);

		staging_texture = m_device.CreateTexture(staging_resource);
		staging_texture->SetName(L"Pano to Cubemap Staging Texture");

		CopyResource(staging_texture, cubemap_texture);
	}

	TransitionBarrier(staging_texture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	SetPipelineState(m_pano_to_cubemap_pso->GetPipelineState());
	SetComputeRootSignature(m_pano_to_cubemap_pso->GetRootSignature());

	PanoToCubemapCB pano_to_cubemap_cb;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
	uav_desc.Format = Texture::GetUAVCompatableFormat(cubemap_desc.Format);
	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	uav_desc.Texture2DArray.FirstArraySlice = 0;
	uav_desc.Texture2DArray.ArraySize = 6;

	auto srv = m_device.CreateShaderResourceView(pano_texture);
	SetShaderResourceView(PanoToCubemapRS::SrcTexture, 0, srv, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	for (uint32_t mip_slice = 0u; mip_slice < cubemap_desc.MipLevels; ) {
		uint32_t num_mips = std::min<uint32_t>(5, cubemap_desc.MipLevels - mip_slice);

		pano_to_cubemap_cb.FirstMip = mip_slice;
		pano_to_cubemap_cb.CubemapSize = std::max<uint32_t>(static_cast<uint32_t>(cubemap_desc.Width), cubemap_desc.Height) >> mip_slice;
		pano_to_cubemap_cb.NumMips = num_mips;

		SetCompute32BitConstants(PanoToCubemapRS::PanoToCubemapCB, pano_to_cubemap_cb);

		for (uint32_t mip = 0u; mip < num_mips; ++mip) {
			uav_desc.Texture2DArray.MipSlice = mip_slice + mip;

			auto uav = m_device.CreateUnorderedAccessView(staging_texture, nullptr, &uav_desc);
			SetUnorderedAccessView(PanoToCubemapRS::DstMips, mip, uav, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, 0, 0);
		}

		if (num_mips < 5u) {
			m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(PanoToCubemapRS::DstMips, pano_to_cubemap_cb.NumMips, 5 - num_mips, m_pano_to_cubemap_pso->GetDefaultUAV());
		}

		Dispatch(Math::DivideByMultiple(pano_to_cubemap_cb.CubemapSize, 16), Math::DivideByMultiple(pano_to_cubemap_cb.CubemapSize, 16), 6);

		mip_slice += num_mips;
	}

	if (staging_resource != cubemap_resource) {
		CopyResource(cubemap_texture, staging_texture);
	}
}

void CommandList::ClearTexture(const std::shared_ptr<Texture>& texture, const float clear_color[4]) {
	assert(texture);

	TransitionBarrier(texture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
	m_d3d12_command_list->ClearRenderTargetView(texture->GetRenderTargetView(), clear_color, 0, nullptr);

	TrackResource(texture);
}

void CommandList::ClearDepthStencilTexture(const std::shared_ptr<Texture>& texture, D3D12_CLEAR_FLAGS clear_flags, float depth, uint8_t stencil) {
	assert(texture);

	TransitionBarrier(texture, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
	m_d3d12_command_list->ClearDepthStencilView(texture->GetDepthStencilView(), clear_flags, depth, stencil, 0, nullptr);

	TrackResource(texture);
}

void CommandList::CopyTextureSubresource(const std::shared_ptr<Texture>& texture, uint32_t first_subresource, uint32_t num_subresources, D3D12_SUBRESOURCE_DATA* subresource_data) {
	assert(texture);

	auto d3d12_device = m_device.GetD3D12Device();
	auto destination_resource = texture->GetD3D12Resource();

	if (destination_resource) {
		TransitionBarrier(texture, D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		UINT64 required_size = GetRequiredIntermediateSize(destination_resource.Get(), first_subresource, num_subresources);

		Microsoft::WRL::ComPtr<ID3D12Resource> intermediate_resource;
		D3D12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(required_size);
		HRESULT hr = d3d12_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(intermediate_resource.GetAddressOf()));
		ThrowIfFailed(hr);

		UpdateSubresources(m_d3d12_command_list.Get(), destination_resource.Get(), intermediate_resource.Get(), 0, first_subresource, num_subresources, subresource_data);

		TrackResource(intermediate_resource);
		TrackResource(destination_resource);
	}
}

void CommandList::SetGraphicsDynamicConstantBuffer(uint32_t root_parameter_index, size_t size_in_bytes, const void* buffer_data) {
	auto heap_allococation = m_upload_buffer->Allocate(size_in_bytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	memcpy(heap_allococation.CPU, buffer_data, size_in_bytes);

	m_d3d12_command_list->SetGraphicsRootConstantBufferView(root_parameter_index, heap_allococation.GPU);
}

void CommandList::SetGraphics32BitConstants(uint32_t root_parameter_index, uint32_t num_constants, const void* constants) {
	m_d3d12_command_list->SetGraphicsRoot32BitConstants(root_parameter_index, num_constants, constants, 0);
}

void CommandList::SetCompute32BitConstants(uint32_t root_parameter_index, uint32_t num_constants, const void* constants) {
	m_d3d12_command_list->SetComputeRoot32BitConstants(root_parameter_index, num_constants, constants, 0);
}

void CommandList::SetVertexBuffers(uint32_t start_slot, const std::vector<std::shared_ptr<VertexBuffer>>& vertex_buffers) {
	std::vector<D3D12_VERTEX_BUFFER_VIEW> views;
	views.reserve(vertex_buffers.size());

	for (auto vertex_buffer : vertex_buffers) {
		if (vertex_buffer) {
			TransitionBarrier(vertex_buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			TrackResource(vertex_buffer);

			views.push_back(vertex_buffer->GetVertexBufferView());
		}
	}

	m_d3d12_command_list->IASetVertexBuffers(start_slot, static_cast<UINT>(views.size()), views.data());
}

void CommandList::SetVertexBuffer(uint32_t slot, const std::shared_ptr<VertexBuffer>& vertex_buffer) {
	SetVertexBuffers(slot, { vertex_buffer });
}

void CommandList::SetDynamicVertexBuffer(uint32_t slot, size_t num_vertices, size_t vertex_size, const void* vertex_buffer_data) {
	size_t buffer_size = num_vertices * vertex_size;

	auto heap_allocation = m_upload_buffer->Allocate(buffer_size, vertex_size);
	memcpy(heap_allocation.CPU, vertex_buffer_data, buffer_size);

	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
	vertex_buffer_view.BufferLocation = heap_allocation.GPU;
	vertex_buffer_view.SizeInBytes = static_cast<UINT>(buffer_size);
	vertex_buffer_view.StrideInBytes = static_cast<UINT>(vertex_size);

	m_d3d12_command_list->IASetVertexBuffers(slot, 1, &vertex_buffer_view);
}

void CommandList::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer) {
	if (index_buffer) {
		TransitionBarrier(index_buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		TrackResource(index_buffer);
		D3D12_INDEX_BUFFER_VIEW ibv = index_buffer->GetIndexBufferView();
		m_d3d12_command_list->IASetIndexBuffer(&ibv);
	}
}

void CommandList::SetDynamicIndexBuffer(size_t num_indices, DXGI_FORMAT index_format, const void* index_buffer_data) {
	size_t index_size_in_bytes = index_format == DXGI_FORMAT_R16_UINT ? 2 : 4;
	size_t buffer_size = num_indices * index_size_in_bytes;

	auto heap_allocation = m_upload_buffer->Allocate(buffer_size, index_size_in_bytes);
	memcpy(heap_allocation.CPU, index_buffer_data, buffer_size);

	D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
	index_buffer_view.BufferLocation = heap_allocation.GPU;
	index_buffer_view.SizeInBytes = static_cast<UINT>(buffer_size);
	index_buffer_view.Format = index_format;

	m_d3d12_command_list->IASetIndexBuffer(&index_buffer_view);
}

void CommandList::SetGraphicsDynamicStructuredBuffer(uint32_t slot, size_t num_elements, size_t element_size, const void* buffer_data) {
	size_t buffer_size = num_elements * element_size;

	auto heap_allocation = m_upload_buffer->Allocate(buffer_size, element_size);

	memcpy(heap_allocation.CPU, buffer_data, buffer_size);

	m_d3d12_command_list->SetGraphicsRootShaderResourceView(slot, heap_allocation.GPU);
}
void CommandList::SetViewport(const D3D12_VIEWPORT& viewport) {
	SetViewports({ viewport });
}

void CommandList::SetViewports(const std::vector<D3D12_VIEWPORT>& viewports) {
	assert(viewports.size() < D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
	m_d3d12_command_list->RSSetViewports(static_cast<UINT>(viewports.size()), viewports.data());
}

void CommandList::SetScissorRect(const D3D12_RECT& scissorRect) {
	SetScissorRects({ scissorRect });
}

void CommandList::SetScissorRects(const std::vector<D3D12_RECT>& scissor_rects) {
	assert(scissor_rects.size() < D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
	m_d3d12_command_list->RSSetScissorRects(static_cast<UINT>(scissor_rects.size()), scissor_rects.data());
}

void CommandList::SetPipelineState(const std::shared_ptr<PipelineStateObject>& pipeline_state) {
	assert(pipeline_state);

	auto d3d12_pipeline_state_object = pipeline_state->GetD3D12PipelineState().Get();
	if (m_pipeline_state != d3d12_pipeline_state_object) {
		m_pipeline_state = d3d12_pipeline_state_object;

		m_d3d12_command_list->SetPipelineState(d3d12_pipeline_state_object);

		TrackResource(d3d12_pipeline_state_object);
	}
}

void CommandList::SetGraphicsRootSignature(const std::shared_ptr<RootSignature>& root_signature) {
	assert(root_signature);

	auto d3d12_root_signature = root_signature->GetD3D12RootSignature().Get();
	if (m_root_signature != d3d12_root_signature) {
		m_root_signature = d3d12_root_signature;

		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
			m_dynamic_descriptor_heap[i]->ParseRootSignature(root_signature);
		}

		m_d3d12_command_list->SetGraphicsRootSignature(m_root_signature);

		TrackResource(m_root_signature);
	}
}

void CommandList::SetComputeRootSignature(const std::shared_ptr<RootSignature>& root_signature) {
	assert(root_signature);

	auto d3d12_root_signature = root_signature->GetD3D12RootSignature().Get();
	if (m_root_signature != d3d12_root_signature) {
		m_root_signature = d3d12_root_signature;

		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
			m_dynamic_descriptor_heap[i]->ParseRootSignature(root_signature);
		}

		m_d3d12_command_list->SetComputeRootSignature(m_root_signature);

		TrackResource(m_root_signature);
	}
}

void CommandList::SetConstantBufferView(uint32_t root_parameter_index, const std::shared_ptr<ConstantBuffer>& buffer, D3D12_RESOURCE_STATES state_after, size_t buffer_offset) {
	if (buffer) {
		auto d3d12_resource = buffer->GetD3D12Resource();
		TransitionBarrier(d3d12_resource, state_after);

		m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageInlineCBV(root_parameter_index, d3d12_resource->GetGPUVirtualAddress() + buffer_offset);

		TrackResource(buffer);
	}
}

void CommandList::SetShaderResourceView(uint32_t root_parameter_index, const std::shared_ptr<Buffer>& buffer, D3D12_RESOURCE_STATES state_after, size_t buffer_offset) {
	if (buffer) {
		auto d3d12_resource = buffer->GetD3D12Resource();
		TransitionBarrier(d3d12_resource, state_after);

		m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageInlineSRV(root_parameter_index, d3d12_resource->GetGPUVirtualAddress() + buffer_offset);

		TrackResource(buffer);
	}
}

void CommandList::SetUnorderedAccessView(uint32_t root_parameter_index, const std::shared_ptr<Buffer>& buffer, D3D12_RESOURCE_STATES state_after, size_t buffer_offset) {
	if (buffer) {
		auto d3d12_resource = buffer->GetD3D12Resource();
		TransitionBarrier(d3d12_resource, state_after);

		m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageInlineUAV(root_parameter_index, d3d12_resource->GetGPUVirtualAddress() + buffer_offset);

		TrackResource(buffer);
	}
}

void CommandList::SetShaderResourceView(uint32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<ShaderResourceView>& srv, D3D12_RESOURCE_STATES state_after, UINT first_subresource, UINT num_subresources) {
	assert(srv);

	auto resource = srv->GetResource();
	if (resource) {
		if (num_subresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
			for (uint32_t i = 0; i < num_subresources; ++i) {
				TransitionBarrier(resource, state_after, first_subresource + i);
			}
		}
		else {
			TransitionBarrier(resource, state_after);
		}

		TrackResource(resource);
	}

	m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(root_parameter_index, descriptor_offset, 1, srv->GetDescriptorHandle());
}

void CommandList::SetShaderResourceView(int32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<Texture>& texture, D3D12_RESOURCE_STATES state_after, UINT first_subresource, UINT num_subresources) {
	if (texture) {
		if (num_subresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
			for (uint32_t i = 0; i < num_subresources; ++i) {
				TransitionBarrier(texture, state_after, first_subresource + i);
			}
		}
		else {
			TransitionBarrier(texture, state_after);
		}

		TrackResource(texture);

		m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(root_parameter_index, descriptor_offset, 1, texture->GetShaderResourceView());
	}
}

void CommandList::SetUnorderedAccessView(uint32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<UnorderedAccessView>& uav, D3D12_RESOURCE_STATES state_after, UINT first_subresource, UINT num_subresources) {
	assert(uav);

	auto resource = uav->GetResource();
	if (resource) {
		if (num_subresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
			for (uint32_t i = 0; i < num_subresources; ++i) {
				TransitionBarrier(resource, state_after, first_subresource + i);
			}
		}
		else {
			TransitionBarrier(resource, state_after);
		}

		TrackResource(resource);
	}

	m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(root_parameter_index, descriptor_offset, 1, uav->GetDescriptorHandle());
}

void CommandList::SetUnorderedAccessView(uint32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<Texture>& texture, UINT mip, D3D12_RESOURCE_STATES state_after, UINT first_subresource, UINT num_subresources) {
	if (texture) {
		if (num_subresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
			for (uint32_t i = 0; i < num_subresources; ++i) {
				TransitionBarrier(texture, state_after, first_subresource + i);
			}
		}
		else {
			TransitionBarrier(texture, state_after);
		}

		TrackResource(texture);

		m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(root_parameter_index, descriptor_offset, 1, texture->GetUnorderedAccessView(mip));
	}
}

void CommandList::SetConstantBufferView(uint32_t root_parameter_index, uint32_t descriptor_offset, const std::shared_ptr<ConstantBufferView>& cbv, D3D12_RESOURCE_STATES state_after) {
	assert(cbv);

	auto constant_buffer = cbv->GetConstantBuffer();
	if (constant_buffer) {
		TransitionBarrier(constant_buffer, state_after);
		TrackResource(constant_buffer);
	}

	m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(root_parameter_index, descriptor_offset, 1, cbv->GetDescriptorHandle());
}

void CommandList::SetRenderTarget(const RenderTarget& render_target) {
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> render_target_descriptors;
	render_target_descriptors.reserve(to_underlying(AttachmentPoint::NumAttachmentPoints));

	const auto& textures = render_target.GetTextures();

	for (int i = 0; i < 8; ++i) {
		auto texture = textures[i];

		if (texture) {
			TransitionBarrier(texture, D3D12_RESOURCE_STATE_RENDER_TARGET);
			render_target_descriptors.push_back(texture->GetRenderTargetView());

			TrackResource(texture);
		}
	}

	auto depth_texture = render_target.GetTexture(AttachmentPoint::DepthStencil);

	CD3DX12_CPU_DESCRIPTOR_HANDLE depth_stencil_descriptor(D3D12_DEFAULT);
	if (depth_texture) {
		TransitionBarrier(depth_texture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		depth_stencil_descriptor = depth_texture->GetDepthStencilView();

		TrackResource(depth_texture);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE* pDSV = depth_stencil_descriptor.ptr != 0 ? &depth_stencil_descriptor : nullptr;

	m_d3d12_command_list->OMSetRenderTargets(static_cast<UINT>(render_target_descriptors.size()), render_target_descriptors.data(), FALSE, pDSV);
}

void CommandList::Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t start_vertex, uint32_t start_instance) {
	FlushResourceBarriers();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamic_descriptor_heap[i]->CommitStagedDescriptorsForDraw(*this);
	}

	m_d3d12_command_list->DrawInstanced(vertex_count, instance_count, start_vertex, start_instance);
}

void CommandList::DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t start_index, int32_t base_vertex, uint32_t start_instance) {
	FlushResourceBarriers();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamic_descriptor_heap[i]->CommitStagedDescriptorsForDraw(*this);
	}

	m_d3d12_command_list->DrawIndexedInstanced(index_count, instance_count, start_index, base_vertex, start_instance);
}

void CommandList::Dispatch(uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z) {
	FlushResourceBarriers();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamic_descriptor_heap[i]->CommitStagedDescriptorsForDispatch(*this);
	}

	m_d3d12_command_list->Dispatch(num_groups_x, num_groups_y, num_groups_z);
}

bool CommandList::Close(const std::shared_ptr<CommandList>& pending_command_list) {
	FlushResourceBarriers();

	m_d3d12_command_list->Close();

	uint32_t num_pending_barriers = m_resource_state_tracker->FlushPendingResourceBarriers(pending_command_list);

	m_resource_state_tracker->CommitFinalResourceStates();

	return num_pending_barriers > 0;
}

void CommandList::Close() {
	FlushResourceBarriers();
	m_d3d12_command_list->Close();
}

void CommandList::Reset() {
	ThrowIfFailed(m_d3d12_command_allocator->Reset());
	ThrowIfFailed(m_d3d12_command_list->Reset(m_d3d12_command_allocator.Get(), nullptr));

	m_resource_state_tracker->Reset();
	m_upload_buffer->Reset();

	ReleaseTrackedObjects();

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		m_dynamic_descriptor_heap[i]->Reset();
		m_descriptor_heaps[i] = nullptr;
	}

	m_root_signature = nullptr;
	m_pipeline_state = nullptr;
	m_compute_command_list = nullptr;
}

void CommandList::TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object) {
	m_tracked_objects.push_back(object);
}

void CommandList::TrackResource(const std::shared_ptr<Resource>& res) {
	assert(res);

	TrackResource(res->GetD3D12Resource());
}

void CommandList::ReleaseTrackedObjects() {
	m_tracked_objects.clear();
}

void CommandList::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, ID3D12DescriptorHeap* heap) {
	if (m_descriptor_heaps[heap_type] != heap) {
		m_descriptor_heaps[heap_type] = heap;
		BindDescriptorHeaps();
	}
}

void CommandList::BindDescriptorHeaps() {
	UINT num_descriptor_heaps = 0;
	ID3D12DescriptorHeap* descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};

	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
		ID3D12DescriptorHeap* descriptor_heap = m_descriptor_heaps[i];
		if (descriptor_heap) {
			descriptor_heaps[num_descriptor_heaps++] = descriptor_heap;
		}
	}

	m_d3d12_command_list->SetDescriptorHeaps(num_descriptor_heaps, descriptor_heaps);
}