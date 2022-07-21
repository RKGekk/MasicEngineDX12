#pragma once

#include "buffer.h"

class IndexBuffer : public Buffer {
public:
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	size_t GetNumIndices() const;

	DXGI_FORMAT GetIndexFormat() const;

protected:
	IndexBuffer(Device& device, size_t num_indices, DXGI_FORMAT index_format);
	IndexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_indices, DXGI_FORMAT index_format);
	virtual ~IndexBuffer() = default;

	void CreateIndexBufferView();

private:
	size_t m_num_indices;
	DXGI_FORMAT m_index_format;
	D3D12_INDEX_BUFFER_VIEW m_index_buffer_view;
};