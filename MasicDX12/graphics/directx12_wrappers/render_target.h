#pragma once

#include <DirectXMath.h>
#include <d3d12.h>

#include <cstdint>
#include <memory>
#include <vector>

class Texture;

enum class AttachmentPoint {
	Color0,
	Color1,
	Color2,
	Color3,
	Color4,
	Color5,
	Color6,
	Color7,
	DepthStencil,
	NumAttachmentPoints
};

class RenderTarget {
public:
	RenderTarget();

	RenderTarget(const RenderTarget& copy) = default;
	RenderTarget(RenderTarget&& copy) = default;

	RenderTarget& operator=(const RenderTarget& other) = default;
	RenderTarget& operator=(RenderTarget&& other) = default;

	void AttachTexture(AttachmentPoint attachment_point, std::shared_ptr<Texture> texture);
	std::shared_ptr<Texture> GetTexture(AttachmentPoint attachment_point) const;

	void Resize(DirectX::XMUINT2 size);
	void Resize(uint32_t width, uint32_t height);
	DirectX::XMUINT2 GetSize() const;
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	D3D12_VIEWPORT GetViewport(DirectX::XMFLOAT2 scale = { 1.0f, 1.0f }, DirectX::XMFLOAT2 bias = { 0.0f, 0.0f }, float min_depth = 0.0f, float maxDepth = 1.0f) const;
	const std::vector<std::shared_ptr<Texture>>& GetTextures() const;
	D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const;
	D3D12_RT_FORMAT_ARRAY GetRenderTargetFormatsWithDepth() const;
	DXGI_FORMAT GetRenderTargetFormat(AttachmentPoint attachment_point) const;
	DXGI_FORMAT GetDepthStencilFormat() const;
	DXGI_SAMPLE_DESC GetSampleDesc() const;

	void Reset();

private:
	using RenderTargetList = std::vector<std::shared_ptr<Texture>>;
	RenderTargetList m_textures;
	DirectX::XMUINT2 m_size;
};