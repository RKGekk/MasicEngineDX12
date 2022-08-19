#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <string>
#include <memory>

class Device;

class Resource {
public:
	Resource(Device& device, const D3D12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE* clear_value = nullptr);
	Resource(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clear_value = nullptr);
	virtual ~Resource() = default;

	Device& GetDevice() const;
	Microsoft::WRL::ComPtr<ID3D12Resource> GetD3D12Resource() const;
	D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;

	void SetName(const std::wstring& name);
	const std::wstring& GetName() const;

	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 format_support) const;
	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 format_support) const;

protected:
	Device& m_device;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12_resource;
	D3D12_FEATURE_DATA_FORMAT_SUPPORT m_format_support;
	std::unique_ptr<D3D12_CLEAR_VALUE> m_d3d12_clear_value;
	std::wstring m_resource_name;

private:
	void CheckFeatureSupport();
};