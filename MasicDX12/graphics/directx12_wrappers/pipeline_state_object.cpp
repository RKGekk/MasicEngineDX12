#include "pipeline_state_object.h"

#include "device.h"
#include "../tools/com_exception.h"
#include "../tools/string_utility.h"

#include <cassert>
#include <utility>

PipelineStateObject::PipelineStateObject(Device& device, std::string name) : m_device(device), m_name(std::move(name)), m_compiled(false) {}

PipelineStateObject::PipelineStateObject(Device& device, std::string name, std::shared_ptr<RootSignature> root_signature) : m_device(device), m_root_signature(root_signature), m_name(std::move(name)), m_compiled(false) {}

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

GraphicsPipelineState::GraphicsPipelineState(Device& device, std::string name) : PipelineStateObject(device, std::move(name)) {}

GraphicsPipelineState::GraphicsPipelineState(Device& device, std::string name, std::shared_ptr<RootSignature> root_signature) : PipelineStateObject(device, std::move(name), root_signature) {}

void GraphicsPipelineState::AddOrReplaceShader(std::shared_ptr<Shader> shader) {
    Shader::Stage stage = shader->GetPipelineStage();
    switch (stage) {
        case Shader::Stage::Vertex: m_vertex_shader = std::static_pointer_cast<VertexShader>(shader); break;
        case Shader::Stage::Hull: m_hull_shader = shader; break;
        case Shader::Stage::Domain: m_domain_shader = shader; break;
        case Shader::Stage::Geometry: m_geometry_shader = shader; break;
        case Shader::Stage::Pixel: m_pixel_shader = std::static_pointer_cast<PixelShader>(shader); break;
        // Graphics pipeline cannot contain compute shader
        case Shader::Stage::Compute: assert(false); break;
        default: assert(false); break;
    }
}

std::shared_ptr<Shader> GraphicsPipelineState::GetShader(Shader::Stage stage) {
    switch (stage) {
        case Shader::Stage::Vertex: return m_vertex_shader; break;
        case Shader::Stage::Hull: return m_hull_shader; break;
        case Shader::Stage::Domain: return m_domain_shader; break;
        case Shader::Stage::Geometry: return m_geometry_shader; break;
        case Shader::Stage::Pixel: return m_pixel_shader; break;
        // Graphics pipeline cannot contain compute shader
        case Shader::Stage::Compute: assert(false); break;
        default: assert(false); break;
    }
    return std::shared_ptr<Shader>();
}

bool GraphicsPipelineState::HaveShader(Shader::Stage stage) {
    bool result = false;
    switch (stage) {
        case Shader::Stage::Vertex: result = m_vertex_shader == nullptr ? false : true; break;
        case Shader::Stage::Hull: result = m_hull_shader == nullptr ? false : true; break;
        case Shader::Stage::Domain: result = m_domain_shader == nullptr ? false : true; break;
        case Shader::Stage::Geometry: result = m_geometry_shader == nullptr ? false : true; break;
        case Shader::Stage::Pixel: result = m_pixel_shader == nullptr ? false : true; break;
        case Shader::Stage::Compute: assert(false); break;
        default: assert(false); break;
    }
    return result;
}

void GraphicsPipelineState::SetVertexShader(std::shared_ptr<VertexShader> vertex_shader) {
    m_vertex_shader = vertex_shader;
}

std::shared_ptr<VertexShader> GraphicsPipelineState::GetVertexShader() {
    return m_vertex_shader;
}

void GraphicsPipelineState::SetPixelShader(std::shared_ptr<PixelShader> pixel_shader) {
    m_pixel_shader = pixel_shader;
}

std::shared_ptr<PixelShader> GraphicsPipelineState::GetPixelShader() {
    return m_pixel_shader;
}

void GraphicsPipelineState::SetDomainShader(std::shared_ptr<Shader> domain_shader) {
    m_domain_shader = domain_shader;
}

std::shared_ptr<Shader> GraphicsPipelineState::GetDomainShader() {
    return m_domain_shader;
}

void GraphicsPipelineState::SetHullShader(std::shared_ptr<Shader> hull_shader) {
    m_hull_shader = hull_shader;
}

std::shared_ptr<Shader> GraphicsPipelineState::GetHullShader() {
    return m_hull_shader;
}

void GraphicsPipelineState::SetGeometryShader(std::shared_ptr<Shader> geometry_shader) {
    m_geometry_shader = geometry_shader;
}

std::shared_ptr<Shader> GraphicsPipelineState::GetGeometryShader() {
    return m_geometry_shader;
}

void GraphicsPipelineState::Compile() {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = m_root_signature->GetD3D12RootSignature().Get();
    if (m_vertex_shader) {
        desc.VS = m_vertex_shader->GetBytecode();
        desc.InputLayout = m_vertex_shader->GetInputAssemblerLayoutDesc();
        desc.PrimitiveTopologyType = m_vertex_shader->GetPrimitiveTopologyType();
    }
    if (m_pixel_shader) {
        if (!m_pixel_shader->GetBypass()) {
            desc.PS = m_pixel_shader->GetBytecode();
        }
        desc.BlendState = m_pixel_shader->GetBlendState().GetState();
        desc.RasterizerState = m_pixel_shader->GetRasterizerState().GetState();
        desc.DepthStencilState = m_pixel_shader->GetDepthStencilState().GetState();
        
        desc.NumRenderTargets = m_pixel_shader->GetRenderTargetCount();
        desc.DSVFormat = m_pixel_shader->GetRenderTargetFormat(AttachmentPoint::DepthStencil);
        for (auto& kv : m_pixel_shader->GetRenderTargetFormats()) {
            auto rt = std::underlying_type<AttachmentPoint>::type(kv.first);
            desc.RTVFormats[rt] = kv.second;
        }
    }
    if (m_domain_shader) desc.DS = m_domain_shader->GetBytecode();
    if (m_hull_shader) desc.HS = m_hull_shader->GetBytecode();
    if (m_geometry_shader) desc.GS = m_geometry_shader->GetBytecode();
    
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.SampleMask = 0xFFFFFFFF;
    desc.NodeMask = m_device.NodeMask();

    HRESULT hr = m_device.GetD3D12Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_d3d12_pipeline_state.ReleaseAndGetAddressOf()));
    ThrowIfFailed(hr);

    m_d3d12_pipeline_state->SetName(to_wstring(m_name).c_str());

    m_compiled = true;
}

ComputePipelineState::ComputePipelineState(Device& device, std::string name) : PipelineStateObject(device, std::move(name)){}

ComputePipelineState::ComputePipelineState(Device& device, std::string name, std::shared_ptr<RootSignature> root_signature) : PipelineStateObject(device, std::move(name), root_signature) {}

void ComputePipelineState::AddOrReplaceShader(std::shared_ptr<Shader> shader) {
    Shader::Stage stage = shader->GetPipelineStage();
    switch (stage) {
        case Shader::Stage::Vertex: assert(false); break;
        case Shader::Stage::Hull: assert(false); break;
        case Shader::Stage::Domain: assert(false); break;
        case Shader::Stage::Geometry: assert(false); break;
        case Shader::Stage::Pixel: assert(false); break;
        case Shader::Stage::Compute: m_compute_shader = shader; break;
        default: assert(false); break;
    }
}

std::shared_ptr<Shader> ComputePipelineState::GetShader(Shader::Stage stage) {
    switch (stage) {
        case Shader::Stage::Vertex: assert(false); break;
        case Shader::Stage::Hull: assert(false); break;
        case Shader::Stage::Domain: assert(false); break;
        case Shader::Stage::Geometry: assert(false); break;
        case Shader::Stage::Pixel: assert(false); break;
        case Shader::Stage::Compute: return m_compute_shader; break;
        default: assert(false); break;
    }
    return std::shared_ptr<Shader>();
}

bool ComputePipelineState::HaveShader(Shader::Stage stage) {
    bool result = false;
    switch (stage) {
        case Shader::Stage::Vertex: assert(false); break;
        case Shader::Stage::Hull: assert(false); break;
        case Shader::Stage::Domain: assert(false); break;
        case Shader::Stage::Geometry: assert(false); break;
        case Shader::Stage::Pixel: assert(false); break;
        case Shader::Stage::Compute: result = m_compute_shader == nullptr ? false : true; break;
        default: assert(false); break;
    }
    return result;
}

void ComputePipelineState::SetComputeShader(std::shared_ptr<Shader> compute_shader) {
    m_compute_shader = compute_shader;
}

std::shared_ptr<Shader> ComputePipelineState::GetComputeShader() {
    return m_compute_shader;
}

void ComputePipelineState::Compile() {
    D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = m_root_signature->GetD3D12RootSignature().Get();;
    desc.CS = m_compute_shader->GetBytecode();
    desc.NodeMask = m_device.NodeMask();

    HRESULT hr = m_device.GetD3D12Device()->CreateComputePipelineState(&desc, IID_PPV_ARGS(m_d3d12_pipeline_state.ReleaseAndGetAddressOf()));
    ThrowIfFailed(hr);

    m_d3d12_pipeline_state->SetName(to_wstring(m_name).c_str());

    m_compiled = true;
}