#pragma once

#include "directx12_wrappers/descriptor_allocation.h"

#include <cstdint>

class Device;
class PipelineStateObject;
class RootSignature;

struct PanoToCubemapCB {
	uint32_t CubemapSize;
	uint32_t FirstMip;
	uint32_t NumMips;
};

namespace PanoToCubemapRS {
	enum {
		PanoToCubemapCB,
		SrcTexture,
		DstMips,
		NumRootParameters
	};
}

class PanoToCubemapPSO {
public:
	PanoToCubemapPSO(Device& device);

	std::shared_ptr<RootSignature> GetRootSignature() const;
	std::shared_ptr<PipelineStateObject> GetPipelineState() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetDefaultUAV() const;

private:
	std::shared_ptr<RootSignature> m_root_signature;
	std::shared_ptr<PipelineStateObject> m_pipeline_state;

	DescriptorAllocation m_default_uav;
};