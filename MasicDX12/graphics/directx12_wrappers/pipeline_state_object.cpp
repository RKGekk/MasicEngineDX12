#include "pipeline_state_object.h"

#include "device.h"
#include "../tools/com_exception.h"

PipelineStateObject::PipelineStateObject(Device& device, const std::string& name) : m_device(device), m_name(name), m_compiled(false) {}

PipelineStateObject::PipelineStateObject(Device& device, const std::string& name, std::shared_ptr<RootSignature> root_signature) : m_device(device), m_root_signature(root_signature), m_name(name), m_compiled(false) {}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineStateObject::GetD3D12PipelineState() {
    if (!m_compiled) Compile();
    return m_d3d12_pipeline_state;
}

std::shared_ptr<RootSignature> PipelineStateObject::GetRootSignature() {
    return m_root_signature;
}

void PipelineStateObject::SetRootSignature(std::shared_ptr<RootSignature> root_signature) {
    m_root_signature = root_signature;
}

const std::string& PipelineStateObject::GetName() const {
    return m_name;
}
