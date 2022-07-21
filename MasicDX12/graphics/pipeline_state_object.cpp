#include "pipeline_state_object.h"

#include "device.h"
#include "../tools/com_exception.h"

PipelineStateObject::PipelineStateObject(Device& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc) : m_device(device) {
    auto d3d12_device = device.GetD3D12Device();
    HRESULT hr = d3d12_device->CreatePipelineState(&desc, IID_PPV_ARGS(m_d3d12_pipeline_state.GetAddressOf()));
    ThrowIfFailed(hr);
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineStateObject::GetD3D12PipelineState() const {
    return m_d3d12_pipeline_state;
}
