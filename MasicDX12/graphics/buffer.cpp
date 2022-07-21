#include "buffer.h"

Buffer::Buffer(Device& device, const D3D12_RESOURCE_DESC& res_desc) : Resource(device, res_desc) {}

Buffer::Buffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource) : Resource(device, resource) {}
