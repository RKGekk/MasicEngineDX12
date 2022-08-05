#pragma once

#include "Buffer.h"

#include <d3d12.h>
#include <wrl.h>

class ConstantBuffer : public Buffer {
public:

	size_t GetSizeInBytes() const;

	ConstantBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource);
	virtual ~ConstantBuffer();

private:
	size_t m_size_in_bytes;
};