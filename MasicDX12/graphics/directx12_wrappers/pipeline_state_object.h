#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>

#include "root_signature.h"

class Device;

class PipelineStateObject {
public:
	PipelineStateObject(Device& device, const std::string& name);
	PipelineStateObject(Device& device, const std::string& name, std::shared_ptr<RootSignature> root_signature);
	virtual ~PipelineStateObject() = default;

	virtual void AddOrReplaceShader() = 0;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetD3D12PipelineState();
	std::shared_ptr<RootSignature> GetRootSignature();
	void SetRootSignature(std::shared_ptr<RootSignature> root_signature);
	const std::string& GetName() const;

private:
	Device& m_device;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d12_pipeline_state;
	std::shared_ptr<RootSignature> m_root_signature;
	std::string m_name;
	bool m_compiled;
};

class GraphicsPipelineState : public PipelineStateObject {
	
};