#pragma once

#include "resource.h"

class Buffer : public Resource {
public:
protected:
	Buffer(Device& device, const D3D12_RESOURCE_DESC& resDesc);
	Buffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource);
};