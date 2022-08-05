#pragma once

#include "descriptor_allocation.h"

#include <d3d12.h>
#include <memory>

class Device;
class Resource;

class UnorderedAccessView {
public:
	std::shared_ptr<Resource> GetResource() const;
	std::shared_ptr<Resource> GetCounterResource() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const;

protected:
	UnorderedAccessView(Device& device, const std::shared_ptr<Resource>& resource, const std::shared_ptr<Resource>& counter_resource = nullptr, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav = nullptr);
	virtual ~UnorderedAccessView() = default;

private:
	Device& m_device;
	std::shared_ptr<Resource> m_resource;
	std::shared_ptr<Resource> m_counter_resource;
	DescriptorAllocation m_descriptor;
};