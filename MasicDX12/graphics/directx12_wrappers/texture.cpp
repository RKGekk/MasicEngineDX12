#include "texture.h"

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include "device.h"
#include "resource_state_tracker.h"
#include "../tools/com_exception.h"

#include <algorithm>

#include <DirectXTex/DirectXTex.h>

Texture::Texture(Device& device, const D3D12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE* clear_value) : Resource(device, resource_desc, clear_value) {
	CreateViews();
}

Texture::Texture(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clear_value) : Resource(device, resource, clear_value) {
	CreateViews();
}

Texture::~Texture() {}

bool Texture::CheckSRVSupport() const {
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
}

bool Texture::CheckRTVSupport() const {
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
}

bool Texture::CheckUAVSupport() const {
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) && CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) && CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
}

bool Texture::CheckDSVSupport() const {
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
}

void Texture::Resize(uint32_t width, uint32_t height, uint32_t depth_or_array_size) {
	if (!m_d3d12_resource) return;

	CD3DX12_RESOURCE_DESC res_desc(m_d3d12_resource->GetDesc());

	res_desc.Width = std::max(width, 1u);
	res_desc.Height = std::max(height, 1u);
	res_desc.DepthOrArraySize = depth_or_array_size;
	res_desc.MipLevels = res_desc.SampleDesc.Count > 1 ? 1 : 0;

	auto d3d12_device = m_device.GetD3D12Device();

	D3D12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	HRESULT hr = d3d12_device->CreateCommittedResource(
		&props, D3D12_HEAP_FLAG_NONE,
		&res_desc,
		D3D12_RESOURCE_STATE_COMMON,
		m_d3d12_clear_value.get(),
		//IID_PPV_ARGS(m_d3d12_resource.GetAddressOf())
		IID_PPV_ARGS(m_d3d12_resource.ReleaseAndGetAddressOf())
	);
	ThrowIfFailed(hr);

	m_d3d12_resource->SetName(m_resource_name.c_str());
	ResourceStateTracker::AddGlobalResourceState(m_d3d12_resource.Get(), D3D12_RESOURCE_STATE_COMMON);

	CreateViews();
}

D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& res_desc, UINT mip_slice, UINT array_slice = 0, UINT plane_slice = 0) {
	D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
	uav_desc.Format = res_desc.Format;

	switch (res_desc.Dimension) {
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if (res_desc.DepthOrArraySize > 1) {
				uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
				uav_desc.Texture1DArray.ArraySize = res_desc.DepthOrArraySize - array_slice;
				uav_desc.Texture1DArray.FirstArraySlice = array_slice;
				uav_desc.Texture1DArray.MipSlice = mip_slice;
			}
			else {
				uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
				uav_desc.Texture1D.MipSlice = mip_slice;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (res_desc.DepthOrArraySize > 1) {
				uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uav_desc.Texture2DArray.ArraySize = res_desc.DepthOrArraySize - array_slice;
				uav_desc.Texture2DArray.FirstArraySlice = array_slice;
				uav_desc.Texture2DArray.PlaneSlice = plane_slice;
				uav_desc.Texture2DArray.MipSlice = mip_slice;
			}
			else {
				uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				uav_desc.Texture2D.PlaneSlice = plane_slice;
				uav_desc.Texture2D.MipSlice = mip_slice;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			uav_desc.Texture3D.WSize = res_desc.DepthOrArraySize - array_slice;
			uav_desc.Texture3D.FirstWSlice = array_slice;
			uav_desc.Texture3D.MipSlice = mip_slice;
			break;
		default:
			throw std::exception("Invalid resource dimension.");
	}

	return uav_desc;
}

void Texture::CreateViews() {
	if (m_d3d12_resource) {
		auto d3d12_device = m_device.GetD3D12Device();

		CD3DX12_RESOURCE_DESC desc(m_d3d12_resource->GetDesc());

		if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && CheckRTVSupport()) {
			m_render_target_view = m_device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE hdl = m_render_target_view.GetDescriptorHandle();
			d3d12_device->CreateRenderTargetView(m_d3d12_resource.Get(), nullptr, hdl);
		}

		if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && CheckDSVSupport()) {
			m_depth_stencil_view = m_device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			D3D12_CPU_DESCRIPTOR_HANDLE hdl = m_depth_stencil_view.GetDescriptorHandle();
			d3d12_device->CreateDepthStencilView(m_d3d12_resource.Get(), nullptr, hdl);
		}

		if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport()) {
			m_shader_resource_view = m_device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			D3D12_CPU_DESCRIPTOR_HANDLE hdl = m_shader_resource_view.GetDescriptorHandle();
			d3d12_device->CreateShaderResourceView(m_d3d12_resource.Get(), nullptr, hdl);
		}

		if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0 && CheckUAVSupport() && desc.DepthOrArraySize == 1) {
			m_unordered_access_view = m_device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, desc.MipLevels);
			for (int i = 0; i < desc.MipLevels; ++i) {
				auto uav_desc = GetUAVDesc(desc, i);
				D3D12_CPU_DESCRIPTOR_HANDLE hdl = m_unordered_access_view.GetDescriptorHandle(i);
				d3d12_device->CreateUnorderedAccessView(m_d3d12_resource.Get(), nullptr, &uav_desc, hdl);
			}
		}
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRenderTargetView() const {
	return m_render_target_view.GetDescriptorHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDepthStencilView() const {
	return m_depth_stencil_view.GetDescriptorHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetShaderResourceView() const {
	return m_shader_resource_view.GetDescriptorHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUnorderedAccessView(uint32_t mip) const {
	return m_unordered_access_view.GetDescriptorHandle(mip);
}

bool Texture::HasAlpha() const {
	DXGI_FORMAT format = GetD3D12ResourceDesc().Format;

	bool has_alpha = false;

	switch (format) {
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
		case DXGI_FORMAT_A8P8:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
			has_alpha = true;
			break;
	}

	return has_alpha;
}

size_t Texture::BitsPerPixel() const {
	auto format = GetD3D12ResourceDesc().Format;
	return DirectX::BitsPerPixel(format);
}

bool Texture::IsUAVCompatibleFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SINT:
			return true;
		default:
			return false;
	}
}

bool Texture::IsSRGBFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return true;
		default:
			return false;
	}
}

bool Texture::IsBGRFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return true;
		default:
			return false;
	}
}

bool Texture::IsDepthFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_D16_UNORM:
			return true;
		default:
			return false;
	}
}

DXGI_FORMAT Texture::GetTypelessFormat(DXGI_FORMAT format) {
	DXGI_FORMAT typeless_format = format;

	switch (format) {
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			typeless_format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
			break;
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			typeless_format = DXGI_FORMAT_R32G32B32_TYPELESS;
			break;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
			typeless_format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
			break;
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
			typeless_format = DXGI_FORMAT_R32G32_TYPELESS;
			break;
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			typeless_format = DXGI_FORMAT_R32G8X24_TYPELESS;
			break;
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
			typeless_format = DXGI_FORMAT_R10G10B10A2_TYPELESS;
			break;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
			typeless_format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
			typeless_format = DXGI_FORMAT_R16G16_TYPELESS;
			break;
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
			typeless_format = DXGI_FORMAT_R32_TYPELESS;
			break;
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
			typeless_format = DXGI_FORMAT_R8G8_TYPELESS;
			break;
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
			typeless_format = DXGI_FORMAT_R16_TYPELESS;
			break;
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
			typeless_format = DXGI_FORMAT_R8_TYPELESS;
			break;
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			typeless_format = DXGI_FORMAT_BC1_TYPELESS;
			break;
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			typeless_format = DXGI_FORMAT_BC2_TYPELESS;
			break;
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			typeless_format = DXGI_FORMAT_BC3_TYPELESS;
			break;
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			typeless_format = DXGI_FORMAT_BC4_TYPELESS;
			break;
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
			typeless_format = DXGI_FORMAT_BC5_TYPELESS;
			break;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			typeless_format = DXGI_FORMAT_B8G8R8A8_TYPELESS;
			break;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			typeless_format = DXGI_FORMAT_B8G8R8X8_TYPELESS;
			break;
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
			typeless_format = DXGI_FORMAT_BC6H_TYPELESS;
			break;
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			typeless_format = DXGI_FORMAT_BC7_TYPELESS;
			break;
	}

	return typeless_format;
}

DXGI_FORMAT Texture::GetSRGBFormat(DXGI_FORMAT format) {
	DXGI_FORMAT srgb_format = format;
	switch (format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			srgb_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC1_UNORM:
			srgb_format = DXGI_FORMAT_BC1_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC2_UNORM:
			srgb_format = DXGI_FORMAT_BC2_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC3_UNORM:
			srgb_format = DXGI_FORMAT_BC3_UNORM_SRGB;
			break;
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			srgb_format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			srgb_format = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC7_UNORM:
			srgb_format = DXGI_FORMAT_BC7_UNORM_SRGB;
			break;
	}

	return srgb_format;
}

DXGI_FORMAT Texture::GetUAVCompatableFormat(DXGI_FORMAT format) {
	DXGI_FORMAT uav_format = format;

	switch (format) {
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			uav_format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
			uav_format = DXGI_FORMAT_R32_FLOAT;
			break;
	}

	return uav_format;
}