#pragma once

#include "buffer.h"
#include "byte_address_buffer.h"

class Device;

class StructuredBuffer : public Buffer {
public:
	virtual size_t GetNumElements() const;
	virtual size_t GetElementSize() const;
	std::shared_ptr<ByteAddressBuffer> GetCounterBuffer() const;

protected:
	StructuredBuffer(Device& device, size_t num_elements, size_t element_size);
	StructuredBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_elements, size_t element_size);

	virtual ~StructuredBuffer() = default;

private:
	size_t m_num_elements;
	size_t m_element_size;

	std::shared_ptr<ByteAddressBuffer> m_counter_buffer;
};