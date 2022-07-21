#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <Windows.h>
#include <wrl.h>

#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <string>

class OutputData {
public:
	OutputData(Microsoft::WRL::ComPtr<IDXGIOutput> pOutput);
	~OutputData();

	OutputData(OutputData&&) = default;

	OutputData() = delete;
	OutputData(const OutputData&) = delete;
	OutputData& operator=(const OutputData&) = delete;
	OutputData& operator=(OutputData&&) = delete;

	Microsoft::WRL::ComPtr<IDXGIOutput> m_pOutput;
	DXGI_OUTPUT_DESC m_description;
	std::vector<DXGI_MODE_DESC> m_modes;
};

class AdapterData {
public:
	using AdapterDataPtr = std::shared_ptr<AdapterData>;

	AdapterData(Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter);
	~AdapterData();

	AdapterData(AdapterData&&) = default;

	AdapterData() = delete;
	AdapterData(const AdapterData&) = delete;
	AdapterData& operator=(const AdapterData&) = delete;
	AdapterData& operator=(AdapterData&&) = delete;

	Microsoft::WRL::ComPtr<IDXGIAdapter4> GetDXGIAdapter();

	const DXGI_ADAPTER_DESC1& GetDescription() const;
	bool GetDX12Capability() const;
	bool GetHardwareSupport() const;
	SIZE_T GetDedicatedVideoMemory() const;

	std::vector<OutputData>& GetOutputs();
	const std::wstring GetTextDescription() const;

private:
	Microsoft::WRL::ComPtr<IDXGIAdapter4> m_pAdapter;
	DXGI_ADAPTER_DESC1 m_description;

	bool m_is_dx12_cap = false;
	bool m_is_hw = false;
	SIZE_T m_dedicated_video_memory = 0u;

	std::vector<OutputData> m_outputs;

private:
	UINT f_get_output_count();
};

class AdapterReader {
public:
	using AdapterDataContainer = std::vector<AdapterData::AdapterDataPtr>;
	using AdapterDataIterator = AdapterDataContainer::iterator;
	using AdapterDataConstIterator = AdapterDataContainer::const_iterator;

	AdapterReader();
	AdapterReader(DXGI_GPU_PREFERENCE gpu_preference);
	AdapterReader(bool use_warp);
	AdapterReader(DXGI_GPU_PREFERENCE gpu_preference, bool use_warp);
	~AdapterReader();

	AdapterReader(const AdapterReader&) = delete;
	AdapterReader(AdapterReader&&) = delete;
	AdapterReader& operator=(const AdapterReader&) = delete;
	AdapterReader& operator=(AdapterReader&&) = delete;

	void Initialize();
	bool CheckTearingSupport();

	std::shared_ptr<AdapterData> GetAdapter();
	Microsoft::WRL::ComPtr<IDXGIFactory6> GetDXGIFactory();

private:
	void f_create_factory();
	void f_fill_adapters();
	UINT f_get_adapters_count(DXGI_GPU_PREFERENCE gpu_preference);

	AdapterDataContainer m_adapters;
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgi_factory;
	DXGI_GPU_PREFERENCE m_gpu_preference;
	bool m_use_warp;
};