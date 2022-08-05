#pragma once

#include <d3d12.h>
#include <wrl.h>

class Device;

class PipelineStateObject {
public:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const;

protected:
	PipelineStateObject(Device& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc);
	virtual ~PipelineStateObject() = default;

private:
	Device& m_device;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d12_pipeline_state;
};