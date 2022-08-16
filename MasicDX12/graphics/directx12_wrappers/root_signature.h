#pragma once

#include <d3dx12.h>
#include <wrl.h>

#include <vector>
#include <memory>
#include <unordered_map>

#include "shader_register.h"
#include "root_parameter.h"
#include "sampler.h"

class Device;

class RootSignature {
public:
	using ParameterIndex = uint32_t;

	RootSignature(Device& device);
	RootSignature(Device& device, const D3D12_ROOT_SIGNATURE_DESC1& root_signature_desc);
	RootSignature(const RootSignature& other);
	virtual ~RootSignature();

	RootSignature& operator=(const RootSignature& right);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetD3D12RootSignature() const;
	const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const;

	uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_type) const;
	uint32_t GetNumDescriptors(uint32_t root_index) const;
	uint32_t GetBytesUsed() const;

	bool ConatinParameterIndex(const SignatureRegisters& location) const;
	ParameterIndex GetParameterIndex(const SignatureRegisters& location) const;

	void AddDescriptorTableParameter(const RootDescriptorTableParameter& table);
	void AddDescriptorParameter(const RootDescriptorParameter& descriptor);
	void AddConstantsParameter(const RootConstantsParameter& constants);
	void AddStaticSampler(const RootSaticSampler& sampler);

private:
	void Destroy();
	D3D12_ROOT_SIGNATURE_DESC1 CombineRootSignatureDesc();
	void SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& root_signature_desc);

	using ParamIndexMap = std::unordered_map<SignatureRegisters, ParameterIndex, LocationHasher, LocationEquality>;

	Device& m_device;
	D3D12_ROOT_SIGNATURE_DESC1 m_root_signature_desc;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature;

	ParamIndexMap m_parameter_indices_map;

	std::vector<RootDescriptorTableParameter> m_descriptor_table_parameters;
	std::vector<RootDescriptorParameter> m_descriptor_parameters;
	std::vector<RootConstantsParameter> m_constant_parameters;
	std::vector<RootSaticSampler> m_root_static_samplers;

	std::vector<D3D12_STATIC_SAMPLER_DESC> m_static_samplers;
	std::vector<D3D12_ROOT_PARAMETER1> m_parameters;

	uint32_t m_num_descriptors_per_table[32];
	uint32_t m_sampler_table_bit_mask;
	uint32_t m_descriptor_table_bit_mask;

	uint32_t m_bytes_used;

	bool m_compiled;
};