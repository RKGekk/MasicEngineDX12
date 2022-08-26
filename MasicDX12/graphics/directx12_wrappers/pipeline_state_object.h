#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>

#include "root_signature.h"
#include "shader.h"

class Device;

class PipelineStateObject {
public:
	PipelineStateObject(Device& device, const std::string& name);
	PipelineStateObject(Device& device, const std::string& name, std::shared_ptr<RootSignature> root_signature);
	virtual ~PipelineStateObject() = default;

	PipelineStateObject() = delete;
	PipelineStateObject(const PipelineStateObject& other) = delete;
	PipelineStateObject& operator=(const PipelineStateObject& rhs) = delete;
	PipelineStateObject(PipelineStateObject&& other) = delete;
	PipelineStateObject& operator=(PipelineStateObject&& rhs) = delete;

	virtual void AddOrReplaceShader(std::shared_ptr<Shader> shader) = 0;
	virtual std::shared_ptr<Shader> GetShader(Shader::Stage stage) = 0;
	virtual bool HaveShader(Shader::Stage stage) = 0;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetD3D12PipelineState();
	std::shared_ptr<RootSignature> GetRootSignature();
	void SetRootSignature(std::shared_ptr<RootSignature> root_signature);
	const std::string& GetName() const;

protected:
	virtual void Compile() = 0;

	bool m_compiled;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d12_pipeline_state;

	Device& m_device;
	
	std::shared_ptr<RootSignature> m_root_signature;
	std::string m_name;
};

class GraphicsPipelineState : public PipelineStateObject {

	virtual void AddOrReplaceShader(std::shared_ptr<Shader> shader) override;
	virtual std::shared_ptr<Shader> GetShader(Shader::Stage stage) override;
	virtual bool HaveShader(Shader::Stage stage) override;

	void SetVertexShader(std::shared_ptr<VertexShader> vertex_shader);
	std::shared_ptr<VertexShader> GetVertexShader();

	void SetPixelShader(std::shared_ptr<PixelShader> pixel_shader);
	std::shared_ptr<PixelShader> GetPixelShader();

	void SetDomainShader(std::shared_ptr<Shader> domain_shader);
	std::shared_ptr<Shader> GetDomainShader();
	
	void SetHullShader(std::shared_ptr<Shader> hull_shader);
	std::shared_ptr<Shader> GetHullShader();

	void SetGeometryShader(std::shared_ptr<Shader> geometry_shader);
	std::shared_ptr<Shader> GetGeometryShader();

protected:
	virtual void Compile() override;
	
private:
	std::shared_ptr<VertexShader> m_vertex_shader;
	std::shared_ptr<PixelShader> m_pixel_shader;
	std::shared_ptr<Shader> m_domain_shader;
	std::shared_ptr<Shader> m_hull_shader;
	std::shared_ptr<Shader> m_geometry_shader;
};

class ComputePipelineState : public PipelineStateObject {

	virtual void AddOrReplaceShader(std::shared_ptr<Shader> shader) override;
	virtual std::shared_ptr<Shader> GetShader(Shader::Stage stage) override;
	virtual bool HaveShader(Shader::Stage stage) override;

	void SetComputeShader(std::shared_ptr<Shader> compute_shader);
	std::shared_ptr<Shader> GetComputeShader();

protected:
	virtual void Compile() override;

private:
	std::shared_ptr<Shader> m_compute_shader;
};