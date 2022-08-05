#pragma once

#include <d3dx12.h>
#include <wrl.h>

#include <vector>
#include <memory>

class Device;

class RootSignature {
public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetD3D12RootSignature() const;

	const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const;

	uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_type) const;
	uint32_t GetNumDescriptors(uint32_t root_index) const;

protected:
	RootSignature(Device& device, const D3D12_ROOT_SIGNATURE_DESC1& root_signature_desc);

	virtual ~RootSignature();

private:
	void Destroy();
	void SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& root_signature_desc);

	Device& m_device;
	D3D12_ROOT_SIGNATURE_DESC1 m_root_signature_desc;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature;

	uint32_t m_num_descriptors_per_table[32];
	uint32_t m_sampler_table_bit_mask;
	uint32_t m_descriptor_table_bit_mask;
};