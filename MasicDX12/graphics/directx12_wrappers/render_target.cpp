#include "render_target.h"

#include "texture.h"
#include "../../tools/memory_utility.h"

#include <algorithm>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

void RenderTarget::Reset() {
    m_textures = RenderTargetList(to_underlying(AttachmentPoint::NumAttachmentPoints));
}

RenderTarget::RenderTarget() : m_textures(to_underlying(AttachmentPoint::NumAttachmentPoints)), m_size(0, 0) {}

void RenderTarget::AttachTexture(AttachmentPoint attachment_point, std::shared_ptr<Texture> texture) {
    m_textures[to_underlying(attachment_point)] = texture;

    if (texture && texture->GetD3D12Resource()) {
        auto desc = texture->GetD3D12ResourceDesc();

        m_size.x = static_cast<uint32_t>(desc.Width);
        m_size.y = static_cast<uint32_t>(desc.Height);
    }
}

std::shared_ptr<Texture> RenderTarget::GetTexture(AttachmentPoint attachment_point) const {
    return m_textures[to_underlying(attachment_point)];
}

void RenderTarget::Resize(DirectX::XMUINT2 size) {
    m_size = size;
    for (auto texture : m_textures) {
        if (texture) {
            texture->Resize(m_size.x, m_size.y);
        }
    }
}
void RenderTarget::Resize(uint32_t width, uint32_t height) {
    Resize(DirectX::XMUINT2(width, height));
}

DirectX::XMUINT2 RenderTarget::GetSize() const {
    return m_size;
}

uint32_t RenderTarget::GetWidth() const {
    return m_size.x;
}

uint32_t RenderTarget::GetHeight() const {
    return m_size.y;
}

D3D12_VIEWPORT RenderTarget::GetViewport(DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 bias, float min_depth, float max_depth) const {
    UINT64 width = 0;
    UINT height = 0;

    for (int i = to_underlying(AttachmentPoint::Color0); i <= to_underlying(AttachmentPoint::Color7); ++i) {
        auto texture = m_textures[i];
        if (texture) {
            auto desc = texture->GetD3D12ResourceDesc();
            width = std::max(width, desc.Width);
            height = std::max(height, desc.Height);
        }
    }

    D3D12_VIEWPORT viewport = {
        (width * bias.x),
        (height * bias.y),
        (width * scale.x),
        (height * scale.y),
        min_depth,
        max_depth
    };

    return viewport;
}

const std::vector<std::shared_ptr<Texture>>& RenderTarget::GetTextures() const {
    return m_textures;
}

D3D12_RT_FORMAT_ARRAY RenderTarget::GetRenderTargetFormats() const {
    D3D12_RT_FORMAT_ARRAY rtv_formats = {};

    for (int i = to_underlying(AttachmentPoint::Color0); i <= to_underlying(AttachmentPoint::Color7); ++i) {
        auto texture = m_textures[i];
        if (texture) {
            rtv_formats.RTFormats[rtv_formats.NumRenderTargets++] = texture->GetD3D12ResourceDesc().Format;
        }
    }

    return rtv_formats;
}

D3D12_RT_FORMAT_ARRAY RenderTarget::GetRenderTargetFormatsWithDepth() const {
    D3D12_RT_FORMAT_ARRAY rtv_formats = {};

    for (int i = to_underlying(AttachmentPoint::Color0); i <= to_underlying(AttachmentPoint::DepthStencil); ++i) {
        auto texture = m_textures[i];
        if (texture) {
            rtv_formats.RTFormats[rtv_formats.NumRenderTargets++] = texture->GetD3D12ResourceDesc().Format;
        }
    }

    return rtv_formats;
}

DXGI_FORMAT RenderTarget::GetRenderTargetFormat(AttachmentPoint attachment_point) const {
    return m_textures[to_underlying(attachment_point)]->GetD3D12ResourceDesc().Format;
}

DXGI_FORMAT RenderTarget::GetDepthStencilFormat() const {
    DXGI_FORMAT dsv_format = DXGI_FORMAT_UNKNOWN;
    auto depth_stencil_texture = m_textures[to_underlying(AttachmentPoint::DepthStencil)];
    if (depth_stencil_texture) {
        dsv_format = depth_stencil_texture->GetD3D12ResourceDesc().Format;
    }

    return dsv_format;
}

DXGI_SAMPLE_DESC RenderTarget::GetSampleDesc() const {
    DXGI_SAMPLE_DESC sample_desc = { 1, 0 };
    for (int i = to_underlying(AttachmentPoint::Color0); i <= to_underlying(AttachmentPoint::Color7); ++i) {
        auto texture = m_textures[i];
        if (texture) {
            sample_desc = texture->GetD3D12ResourceDesc().SampleDesc;
            break;
        }
    }

    return sample_desc;
}