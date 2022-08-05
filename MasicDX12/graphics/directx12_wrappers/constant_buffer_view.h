#pragma once

#include "descriptor_allocation.h"

#include <d3d12.h>
#include <memory>

class ConstantBuffer;
class Device;

class ConstantBufferView {
public:
	std::shared_ptr<ConstantBuffer> GetConstantBuffer() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle();

protected:
	ConstantBufferView(Device& device, const std::shared_ptr<ConstantBuffer>& constant_buffer, size_t offset = 0);
	virtual ~ConstantBufferView() = default;

private:
	Device& m_device;
	std::shared_ptr<ConstantBuffer> m_constant_buffer;
	DescriptorAllocation m_descriptor;
};