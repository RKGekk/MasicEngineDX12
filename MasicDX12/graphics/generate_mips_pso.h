#pragma once

#include "directx12_wrappers/descriptor_allocation.h"

#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

class Device;
class PipelineStateObject;
class RootSignature;

struct alignas(16) GenerateMipsCB {
	uint32_t SrcMipLevel;
	uint32_t NumMipLevels;
	uint32_t SrcDimension;
	uint32_t IsSRGB;
	DirectX::XMFLOAT2 TexelSize;
};

namespace GenerateMips {
	enum {
		GenerateMipsCB,
		SrcMip,
		OutMip,
		NumRootParameters
	};
}

class GenerateMipsPSO {
public:
	GenerateMipsPSO(Device& device);

	std::shared_ptr<RootSignature> GetRootSignature() const;
	std::shared_ptr<PipelineStateObject> GetPipelineState() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDefaultUAV() const;

private:
	std::shared_ptr<RootSignature> m_root_signature;
	std::shared_ptr<PipelineStateObject> m_pipeline_state;

	DescriptorAllocation m_default_uav;
};
