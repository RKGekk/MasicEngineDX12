#pragma once

#include <string>

#include <d3dcompiler.h>
#include <d3dx12.h>
#include <wrl/client.h>

#include "root_signature.h"

class Shader {
public:
	enum class Stage { Vertex, Hull, Domain, Geometry, Pixel, Compute };

	Shader(const Microsoft::WRL::ComPtr<ID3DBlob>& blob, const std::string& entry_point, Stage stage, const std::string& name);
	Shader(const std::string& entry_point, Stage stage, const std::string& name, std::shared_ptr<RootSignature> root_sign);

	void SetName(const std::string& name);
	const std::string& GetName() const;

	const Microsoft::WRL::ComPtr<ID3DBlob>& GetBlob() const;
	D3D12_SHADER_BYTECODE GetBytecode() const;
	const std::string& GetEntryPoint() const;
	const Stage GetPipelineStage() const;
	
private:
	std::string m_name;
	std::string m_entry_point;
	Stage m_stage;
	
	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
	std::shared_ptr<RootSignature> m_root_signature;
};