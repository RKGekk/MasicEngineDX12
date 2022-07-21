#pragma once

#include "Buffer.h"

class VertexBuffer : public Buffer {
public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

	size_t GetNumVertices() const;
	size_t GetVertexStride() const;

protected:
	VertexBuffer(Device& device, size_t num_vertices, size_t vertex_stride);
	VertexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_vertices, size_t vertex_stride);
	virtual ~VertexBuffer();

	void CreateVertexBufferView();

private:
	size_t m_num_vertices;
	size_t m_vertex_stride;
	D3D12_VERTEX_BUFFER_VIEW m_vertex_buffer_view;
};