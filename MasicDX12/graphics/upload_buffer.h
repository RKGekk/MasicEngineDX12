#pragma once

#include "../tools/defines.h"

#include <d3d12.h>
#include <wrl.h>

#include <deque>
#include <memory>

class Device;

class UploadBuffer {
public:
	struct Allocation {
		void* CPU;
		D3D12_GPU_VIRTUAL_ADDRESS GPU;
	};

	size_t GetPageSize() const;
	Allocation Allocate(size_t size_in_bytes, size_t alignment);
	void Reset();

	explicit UploadBuffer(Device& device, size_t page_size = _2MB);
	virtual ~UploadBuffer();

private:
	struct Page {
		Page(Device& device, size_t size_in_bytes);
		~Page();

		bool HasSpace(size_t size_in_bytes, size_t alignment) const;
		Allocation Allocate(size_t size_in_bytes, size_t alignment);
		void Reset();

	private:
		Device& m_device;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12_resource;

		void* m_cpu_ptr;
		D3D12_GPU_VIRTUAL_ADDRESS m_gpu_ptr;
		size_t m_page_size;
		size_t m_offset;
	};

	using PagePool = std::deque<std::shared_ptr<Page>>;

	Device& m_device;
	std::shared_ptr<Page> RequestPage();

	PagePool m_page_pool;
	PagePool m_available_pages;

	std::shared_ptr<Page> m_current_page;

	size_t m_page_size;
};