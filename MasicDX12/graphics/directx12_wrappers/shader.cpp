#include "shader.h"

#include "../../tools/string_utility.h"
#include "../../tools/com_exception.h"

Shader::Shader(const std::string& entry_point, Stage stage, const std::string& name, std::shared_ptr<RootSignature> root_sign) : m_entry_point(entry_point), m_stage(stage), m_name(name), m_root_signature(root_sign) {
	HRESULT hr = D3DReadFileToBlob(to_wstring(name).c_str(), m_blob.GetAddressOf());
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
