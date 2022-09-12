#pragma once

#include "descriptor_allocation.h"
#include "resource.h"

#include <directx/d3dx12.h>

#include <mutex>
#include <unordered_map>

class Device;

class Texture : public Resource {
public:

	void Resize(uint32_t width, uint32_t height, uint32_t depth_or_array_size = 1u);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mip) const;

	bool CheckSRVSupport() const;
	bool CheckRTVSupport() const;
	bool CheckUAVSupport() const;
	bool CheckDSVSupport() const;

	bool HasAlpha() const;

	size_t BitsPerPixel() const;

	static bool IsUAVCompatibleFormat(DXGI_FORMAT format);
	static bool IsSRGBFormat(DXGI_FORMAT format);
	static bool IsBGRFormat(DXGI_FORMAT format);
	static bool IsDepthFormat(DXGI_FORMAT format);

	static DXGI_FORMAT GetTypelessFormat(DXGI_FORMAT format);
	static DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT format);
	static DXGI_FORMAT GetUAVCompatableFormat(DXGI_FORMAT format);

protected:
	Texture(Device& device, const D3D12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE* clear_value = nullptr);
	Texture(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clear_value = nullptr);
	virtual ~Texture();

	void CreateViews();

private:
	DescriptorAllocation m_render_target_view;
	DescriptorAllocation m_depth_stencil_view;
	DescriptorAllocation m_shader_resource_view;
	DescriptorAllocation m_unordered_access_view;
};