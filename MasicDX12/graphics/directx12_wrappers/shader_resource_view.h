#pragma once

#include "descriptor_allocation.h"

#include <d3d12.h>
#include <memory>

class Device;
class Resource;

class ShaderResourceView {
public:
	std::shared_ptr<Resource> GetResource() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const;

protected:
	ShaderResourceView(Device& device, const std::shared_ptr<Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr);
	virtual ~ShaderResourceView() = default;

private:
	Device& m_device;
	std::shared_ptr<Resource> m_resource;
	DescriptorAllocation m_descriptor;
};