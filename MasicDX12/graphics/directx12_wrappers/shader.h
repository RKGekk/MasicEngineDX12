#pragma once

#include <string>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>
#include <optional>

#include <d3dcompiler.h>
#include <d3dx12.h>
#include <wrl/client.h>

#include "shader_register.h"
#include "input_assembler_layout.h"
#include "render_target.h"
#include "primitive_topology.h"

class Shader {
public:
	using ShaderRegistersSet = std::unordered_set<SignatureRegisters, LocationHasher, LocationEquality>;
	enum class Stage { Vertex, Hull, Domain, Geometry, Pixel, Compute };

	Shader(Microsoft::WRL::ComPtr<ID3DBlob> blob, const std::string& entry_point, Stage stage, const std::string& name);
	Shader(const std::filesystem::path& executable_folder, const std::string& entry_point, Stage stage, const std::string& name);
	virtual ~Shader() = default;

	void SetName(const std::string& name);
	const std::string& GetName() const;

	const Microsoft::WRL::ComPtr<ID3DBlob>& GetBlob() const;
	D3D12_SHADER_BYTECODE GetBytecode() const;
	const std::string& GetEntryPoint() const;
	const Stage GetPipelineStage() const;

	const ShaderRegistersSet GetRegisters() const;
	void AddRegister(SignatureRegisters register_location);
	void RemoveRegister(SignatureRegisters register_location);
	
protected:
	std::string m_name;
	std::string m_entry_point;
	Stage m_stage;

	ShaderRegistersSet m_registers;
	
	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;

	static const std::string m_default_vertex_entry_point;
	static const std::string m_default_pixel_entry_point;
	static const std::string m_default_geometry_entry_point;
	static const std::string m_default_compute_entry_point;
	static const std::string m_default_ray_generation_entry_point;
	static const std::string m_default_ray_miss_entry_point;
	static const std::string m_default_ray_any_hit_entry_point;
	static const std::string m_default_ray_closest_hit_entry_point;
	static const std::string m_default_ray_intersec_entry_point;
};

class VertexShader : public Shader {
public:
	VertexShader(Microsoft::WRL::ComPtr<ID3DBlob> blob, const std::string& entry_point, const std::string& name);
	VertexShader(const std::filesystem::path& executable_folder, const std::string& entry_point, const std::string& name);

	void SetInputAssemblerLayout(const InputAssemblerLayout& layout);
	void SetInputAssemblerLayout(const D3D12_INPUT_LAYOUT_DESC& layout);
	InputAssemblerLayout& GetInputAssemblerLayout();
	const D3D12_INPUT_LAYOUT_DESC& GetInputAssemblerLayoutDesc() const;

	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
	D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology();
	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType();

private:
	InputAssemblerLayout m_input_layout;
	PrimitiveTopology m_primitive_topology;
};

class PixelShader : public Shader {
public:
	PixelShader(Microsoft::WRL::ComPtr<ID3DBlob> blob, const std::string& entry_point, const std::string& name);
	PixelShader(const std::filesystem::path& executable_folder, const std::string& entry_point, const std::string& name);

	

private:
	std::unordered_map<AttachmentPoint, DXGI_FORMAT> m_render_target_formats_map;
};