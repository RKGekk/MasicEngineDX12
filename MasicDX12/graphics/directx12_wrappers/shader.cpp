#include "shader.h"

#include "../../tools/string_utility.h"
#include "../../tools/com_exception.h"

const std::string Shader::m_default_vertex_entry_point = "VSMain";
const std::string Shader::m_default_pixel_entry_point = "PSMain";
const std::string Shader::m_default_geometry_entry_point = "GSMain";
const std::string Shader::m_default_compute_entry_point = "CSMain";
const std::string Shader::m_default_ray_generation_entry_point = "RayGeneration";
const std::string Shader::m_default_ray_miss_entry_point = "RayMiss";
const std::string Shader::m_default_ray_any_hit_entry_point = "RayAnyHit";
const std::string Shader::m_default_ray_closest_hit_entry_point = "RayClosestHit";
const std::string Shader::m_default_ray_intersec_entry_point = "RayIntersection";

Shader::Shader(Microsoft::WRL::ComPtr<ID3DBlob> blob, const std::string& entry_point, Stage stage, const std::string& name) : m_entry_point(entry_point), m_stage(stage), m_name(name), m_blob(blob) {}

Shader::Shader(const std::filesystem::path& executable_folder, const std::string& entry_point, Stage stage, const std::string& name) : m_entry_point(entry_point), m_stage(stage), m_name(name) {
	std::wstring file_name = (executable_folder / name).wstring();
	HRESULT hr = D3DReadFileToBlob(file_name.c_str(), m_blob.GetAddressOf());
	ThrowIfFailed(hr);
}

void Shader::SetName(const std::string& name) {
	m_name = name;
}

const std::string& Shader::GetName() const {
	return m_name;
}

const Microsoft::WRL::ComPtr<ID3DBlob>& Shader::GetBlob() const {
	return m_blob;
}

D3D12_SHADER_BYTECODE Shader::GetBytecode() const {
	return { m_blob->GetBufferPointer(), m_blob->GetBufferSize() };
}

const std::string& Shader::GetEntryPoint() const {
	return m_entry_point;
}

const Shader::Stage Shader::GetPipelineStage() const {
	return m_stage;
}

const Shader::ShaderRegistersSet Shader::GetRegisters() const {
	return m_registers;
}

void Shader::AddRegister(SignatureRegisters register_location) {
	m_registers.insert(register_location);
}

void Shader::RemoveRegister(SignatureRegisters register_location) {
	m_registers.erase(register_location);
}

bool Shader::IsRegisterFree(SignatureRegisters register_location) {
	return !m_registers.count(register_location);
}

VertexShader::VertexShader(Microsoft::WRL::ComPtr<ID3DBlob> blob, const std::string& entry_point, const std::string& name) : Shader(blob, entry_point, Shader::Stage::Vertex, name) {}

VertexShader::VertexShader(const std::filesystem::path& executable_folder, const std::string& entry_point, const std::string& name) : Shader(executable_folder, entry_point, Shader::Stage::Vertex, name) {}

void VertexShader::SetInputAssemblerLayout(const InputAssemblerLayout& layout) {
	m_input_layout = layout;
}

void VertexShader::SetInputAssemblerLayout(const D3D12_INPUT_LAYOUT_DESC& layout) {
	m_input_layout.SetInputAssemblerLayout(layout);
}

InputAssemblerLayout& VertexShader::GetInputAssemblerLayout() {
	return m_input_layout;
}

const D3D12_INPUT_LAYOUT_DESC& VertexShader::GetInputAssemblerLayoutDesc() const {
	return m_input_layout.GetLayout();
}

void VertexShader::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) {
	m_primitive_topology.SetPrimitiveTopology(topology);
}

void VertexShader::SetPrimitiveTopology(PrimitiveTopology topology) {
	m_primitive_topology = topology;
}

D3D12_PRIMITIVE_TOPOLOGY VertexShader::GetPrimitiveTopology() const {
	return m_primitive_topology.GetPrimitiveTopology();
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE VertexShader::GetPrimitiveTopologyType() const {
	return m_primitive_topology.GetPrimitiveTopologyType();
}

const PrimitiveTopology& VertexShader::GetPrimitiveTopologyClass() const {
	return m_primitive_topology;
}

PixelShader::PixelShader(Microsoft::WRL::ComPtr<ID3DBlob> blob, const std::string& entry_point, const std::string& name) : Shader(blob, entry_point, Shader::Stage::Pixel, name) {};

PixelShader::PixelShader(const std::filesystem::path& executable_folder, const std::string& entry_point, const std::string& name) : Shader(executable_folder, entry_point, Shader::Stage::Pixel, name) {}

const PixelShader::RenderTargetFormatMap& PixelShader::GetRenderTargetFormats() const {
	return m_render_target_formats_map;
}

DXGI_FORMAT PixelShader::GetRenderTargetFormat(AttachmentPoint render_target) const {
	return m_render_target_formats_map.at(render_target);
}

UINT PixelShader::GetRenderTargetCount() const {
	UINT result = 0u;
	if (m_render_target_formats_map.count(AttachmentPoint::DepthStencil)) {
		result = (UINT)(m_render_target_formats_map.size() - 1u);
	}
	else {
		result = (UINT)m_render_target_formats_map.size();
	}
	return result;
}

bool PixelShader::IsRenderTargetFormatSet(AttachmentPoint render_target) const {
	m_render_target_formats_map.count(render_target);
}

void PixelShader::EraseRenderTargetFormat(AttachmentPoint render_target) {
	if (m_render_target_formats_map.count(render_target)) {
		m_render_target_formats_map.erase(render_target);
	}
}

void PixelShader::SetRenderTargetFormat(AttachmentPoint render_target, DXGI_FORMAT format) {
	m_render_target_formats_map[render_target] = format;
}

void PixelShader::SetBlendState(const BlendState& state) {
	m_blend_state = state;
}

const BlendState& PixelShader::GetBlendState() const {
	return m_blend_state;
}

void PixelShader::SetRasterizerState(const RasterizerState& state) {
	m_rasterizer_state = state;
}

const RasterizerState& PixelShader::GetRasterizerState() const {
	return m_rasterizer_state;
}

void PixelShader::SetDepthStencilState(const DepthStencilState& state) {
	m_depth_stencil_state = state;
}

const DepthStencilState& PixelShader::GetDepthStencilState() const {
	return m_depth_stencil_state;
}
