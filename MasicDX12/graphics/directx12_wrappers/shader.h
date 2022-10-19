#pragma once

#include <string>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>
#include <optional>

#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include <wrl/client.h>

#include "shader_register.h"
#include "input_assembler_layout.h"
#include "render_target.h"
#include "primitive_topology.h"
#include "blend_state.h"
#include "rasterizer_state.h"
#include "depth_stencil_state.h"

class Shader {
public:
	using ShaderLocationName = std::string;
	using ShaderRegistersSet = std::unordered_map<SignatureRegisters, ShaderLocationName, LocationHasher, LocationEquality>;
	enum class Stage { Vertex, Hull, Domain, Geometry, Pixel, Compute };

	Shader(Microsoft::WRL::ComPtr<ID3DBlob> blob, std::string entry_point, Stage stage, std::string name);
	Shader(const std::filesystem::path& executable_folder, std::string entry_point, Stage stage, std::string name);
	virtual ~Shader() = default;

	void SetName(std::string name);
	const std::string& GetName() const;

	const Microsoft::WRL::ComPtr<ID3DBlob>& GetBlob() const;
	D3D12_SHADER_BYTECODE GetBytecode() const;
	const std::string& GetEntryPoint() const;
	const Stage GetPipelineStage() const;

	const ShaderRegistersSet& GetRegisters() const;
	void AddRegister(SignatureRegisters register_location, std::string name);
	void RemoveRegister(SignatureRegisters register_location);
	bool IsRegisterFree(SignatureRegisters register_location);
	
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
	void SetPrimitiveTopology(PrimitiveTopology topology);
	D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType() const;
	const PrimitiveTopology& GetPrimitiveTopologyClass() const;

private:
	InputAssemblerLayout m_input_layout;
	PrimitiveTopology m_primitive_topology;
};

class PixelShader : public Shader {
public:
	using RenderTargetFormatMap = std::unordered_map<AttachmentPoint, DXGI_FORMAT>;

	PixelShader(Microsoft::WRL::ComPtr<ID3DBlob> blob, const std::string& entry_point, const std::string& name, bool bypass = false);
	PixelShader(const std::filesystem::path& executable_folder, const std::string& entry_point, const std::string& name, bool bypass = false);

	DXGI_FORMAT GetRenderTargetFormat(AttachmentPoint render_target) const;
	UINT GetRenderTargetCount() const;
	const RenderTargetFormatMap& GetRenderTargetFormats() const;
	void SetRenderTargetFormat(AttachmentPoint render_target, DXGI_FORMAT format);
	void SetRenderTargetFormat(D3D12_RT_FORMAT_ARRAY rtv_formats);
	void EraseRenderTargetFormat(AttachmentPoint render_target);
	bool IsRenderTargetFormatSet(AttachmentPoint render_target) const;

	void SetBlendState(const BlendState& state);
	void SetBlendState(const D3D12_BLEND_DESC& desc);
	const BlendState& GetBlendState() const;

	void SetRasterizerState(const RasterizerState& state);
	void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
	const RasterizerState& GetRasterizerState() const;

	void SetDepthStencilState(const DepthStencilState& state);
	void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);
	const DepthStencilState& GetDepthStencilState() const;

	void SetSample(const DXGI_SAMPLE_DESC& desc);
	const DXGI_SAMPLE_DESC& GetSample() const;

	bool GetBypass() const;

private:
	RenderTargetFormatMap m_render_target_formats_map;
	BlendState m_blend_state;
	RasterizerState m_rasterizer_state;
	DepthStencilState m_depth_stencil_state;
	DXGI_SAMPLE_DESC m_sample;
	bool m_bypass;
};