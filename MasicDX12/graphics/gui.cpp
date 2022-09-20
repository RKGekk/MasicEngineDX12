#include "gui.h"

#include <string>

#include "directx12_wrappers/command_list.h"
#include "directx12_wrappers/command_queue.h"
#include "directx12_wrappers/device.h"
#include "directx12_wrappers/render_target.h"
#include "directx12_wrappers/root_signature.h"
#include "directx12_wrappers/shader_resource_view.h"
#include "directx12_wrappers/pipeline_state_object.h"
#include "directx12_wrappers/texture.h"
#include "directx12_wrappers/shader.h"
#include "../tools/com_exception.h"

#include <../graphics/imgui/imgui_impl_win32.h>
#include <d3dcompiler.h>
#include <DirectXTex/DirectXTex.h>

enum RootParameters {
    MatrixCB,
    FontTexture,
    NumRootParameters
};

void GetSurfaceInfo(_In_ size_t width, _In_ size_t height, _In_ DXGI_FORMAT fmt, size_t* out_num_bytes, _Out_opt_ size_t* out_row_bytes, _Out_opt_ size_t* out_num_rows);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GUI::GUI(Device& device, HWND hWnd, const RenderTarget& render_target) : m_device(device), m_hWnd(hWnd), m_pImgui_ctx(nullptr) {
    using namespace std::literals;
    m_pImgui_ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_pImgui_ctx);
    if (!ImGui_ImplWin32_Init(m_hWnd)) {
        throw std::exception("Failed to initialize ImGui");
    }

    ImGuiIO& io = ImGui::GetIO();

    io.FontGlobalScale = ::GetDpiForWindow(m_hWnd) / 96.0f;
    io.FontAllowUserScaling = true;

    unsigned char* pixel_data = nullptr;
    int width;
    int height;
    io.Fonts->GetTexDataAsRGBA32(&pixel_data, &width, &height);

    auto& command_queue = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto command_list = command_queue.GetCommandList();

    auto font_texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

    m_font_texture = m_device.CreateTexture(font_texture_desc);
    m_font_texture->SetName(L"ImGui Font Texture");
    m_font_srv = m_device.CreateShaderResourceView(m_font_texture);

    size_t row_pitch;
    size_t slice_pitch;
    GetSurfaceInfo(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, &slice_pitch, &row_pitch, nullptr);

    D3D12_SUBRESOURCE_DATA subresource_data;
    subresource_data.pData = pixel_data;
    subresource_data.RowPitch = row_pitch;
    subresource_data.SlicePitch = slice_pitch;

    command_list->CopyTextureSubresource(m_font_texture, 0, 1, &subresource_data);
    command_list->GenerateMips(m_font_texture);

    command_queue.ExecuteCommandList(command_list);

    auto d3d12_device = m_device.GetD3D12Device();

    D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
    CD3DX12_DESCRIPTOR_RANGE1 descriptor_rage(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_ROOT_PARAMETER1 root_parameters[RootParameters::NumRootParameters];
    root_parameters[RootParameters::MatrixCB].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    root_parameters[RootParameters::FontTexture].InitAsDescriptorTable(1, &descriptor_rage, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_STATIC_SAMPLER_DESC linear_repeat_sampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT);
    linear_repeat_sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    linear_repeat_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description;
    root_signature_description.Init_1_1(RootParameters::NumRootParameters, root_parameters, 1, &linear_repeat_sampler, root_signature_flags);

    m_root_signature = m_device.CreateRootSignature("GUIRootSignature"s, root_signature_description);

    const D3D12_INPUT_ELEMENT_DESC input_layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(ImDrawVert, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(ImDrawVert, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(ImDrawVert, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_BLEND_DESC blend_desc = {};
    blend_desc.RenderTarget[0].BlendEnable = true;
    blend_desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_RASTERIZER_DESC rasterizer_desc = {};
    rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer_desc.DepthClipEnable = true;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = {};
    depth_stencil_desc.DepthEnable = false;
    depth_stencil_desc.StencilEnable = false;

    static Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader_blob;
    if (!vertex_shader_blob) {
        HRESULT hr = D3DReadFileToBlob(L"ImGUI_VS.cso", vertex_shader_blob.GetAddressOf());
        ThrowIfFailed(hr);
    }
    m_vertex_shader = std::make_shared<VertexShader>(vertex_shader_blob, "main"s, "ImGUI_VS.cso"s);
    m_vertex_shader->AddRegister({ 0, 0, ShaderRegister::ConstantBuffer }, "vertexBufferProjectionMatrix"s);
    m_vertex_shader->SetInputAssemblerLayout({ input_layout, 3 });
    m_vertex_shader->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

    static Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader_blob;
    if (!pixel_shader_blob) {
        HRESULT hr = D3DReadFileToBlob(L"ImGUI_PS.cso", pixel_shader_blob.GetAddressOf());
        ThrowIfFailed(hr);
    }
    m_pixel_shader = std::make_shared<PixelShader>(pixel_shader_blob, "main"s, "ImGUI_PS.cso"s);
    m_pixel_shader->AddRegister({ 0, 0, ShaderRegister::Sampler }, "sampler0"s);
    m_pixel_shader->AddRegister({ 0, 0, ShaderRegister::ShaderResource }, "DiffuseTexture"s);
    m_pixel_shader->SetRenderTargetFormat(render_target.GetRenderTargetFormats());
    m_pixel_shader->SetRenderTargetFormat(AttachmentPoint::DepthStencil, render_target.GetDepthStencilFormat());
    m_pixel_shader->SetSample(render_target.GetSampleDesc());
    m_pixel_shader->SetBlendState(CD3DX12_BLEND_DESC(blend_desc));
    m_pixel_shader->SetRasterizerState(CD3DX12_RASTERIZER_DESC(rasterizer_desc));
    m_pixel_shader->SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(depth_stencil_desc));

    //m_pipeline_state = std::dynamic_pointer_cast<PipelineStateObject>(device.CreateGraphicsPipelineState("ImGuiPSO"s, m_root_signature, m_vertex_shader, m_pixel_shader));
    m_pipeline_state = device.CreateGraphicsPipelineState("ImGuiPSO"s, m_root_signature, m_vertex_shader, m_pixel_shader);
}

GUI::~GUI() {
    Destroy();
}

void GUI::NewFrame() {
    ImGui::SetCurrentContext(m_pImgui_ctx);
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void GUI::Render(const std::shared_ptr<CommandList>& command_list, const RenderTarget& render_target) {
    assert(command_list);

    ImGui::SetCurrentContext(m_pImgui_ctx);
    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    ImDrawData* draw_data = ImGui::GetDrawData();

    if (!draw_data || draw_data->CmdListsCount == 0) return;

    ImVec2 display_pos = draw_data->DisplayPos;

    command_list->SetPipelineState(m_pipeline_state);
    command_list->SetGraphicsRootSignature(m_root_signature);
    command_list->SetRenderTarget(render_target);

    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    float mvp[4][4] = {
        { 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
        { 0.0f, 2.0f / (T - B), 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.5f, 0.0f },
        { (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f },
    };

    command_list->SetGraphics32BitConstants(RootParameters::MatrixCB, mvp);
    command_list->SetShaderResourceView(RootParameters::FontTexture, 0, m_font_srv, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    D3D12_VIEWPORT viewport = {};
    viewport.Width = draw_data->DisplaySize.x;
    viewport.Height = draw_data->DisplaySize.y;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    command_list->SetViewport(viewport);
    command_list->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const DXGI_FORMAT indexFormat = sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

    command_list->FlushResourceBarriers();

    for (int i = 0; i < draw_data->CmdListsCount; ++i) {
        const ImDrawList* draw_list = draw_data->CmdLists[i];

        command_list->SetDynamicVertexBuffer(0, draw_list->VtxBuffer.size(), sizeof(ImDrawVert), draw_list->VtxBuffer.Data);
        command_list->SetDynamicIndexBuffer(draw_list->IdxBuffer.size(), indexFormat, draw_list->IdxBuffer.Data);

        int index_offset = 0;
        for (int j = 0; j < draw_list->CmdBuffer.size(); ++j) {
            const ImDrawCmd& draw_cmd = draw_list->CmdBuffer[j];
            if (draw_cmd.UserCallback) {
                draw_cmd.UserCallback(draw_list, &draw_cmd);
            }
            else {
                ImVec4 clip_rect = draw_cmd.ClipRect;
                D3D12_RECT scissor_rect;
                scissor_rect.left = static_cast<LONG>(clip_rect.x - display_pos.x);
                scissor_rect.top = static_cast<LONG>(clip_rect.y - display_pos.y);
                scissor_rect.right = static_cast<LONG>(clip_rect.z - display_pos.x);
                scissor_rect.bottom = static_cast<LONG>(clip_rect.w - display_pos.y);

                if (scissor_rect.right - scissor_rect.left > 0.0f && scissor_rect.bottom - scissor_rect.top > 0.0) {
                    command_list->SetScissorRect(scissor_rect);
                    command_list->DrawIndexed(draw_cmd.ElemCount, 1, index_offset);
                }
            }
            index_offset += draw_cmd.ElemCount;
        }
    }
}

void GUI::Destroy() {
    ImGui::EndFrame();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(m_pImgui_ctx);
    m_pImgui_ctx = nullptr;
}

void GUI::SetScaling(float scale) {
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = scale;
}

LRESULT GUI::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

void GetSurfaceInfo(_In_ size_t width, _In_ size_t height, _In_ DXGI_FORMAT fmt, size_t* out_num_bytes, _Out_opt_ size_t* out_row_bytes, _Out_opt_ size_t* out_num_rows) {
    size_t num_bytes = 0u;
    size_t row_bytes = 0u;
    size_t num_rows = 0u;

    bool bc = false;
    bool packed = false;
    bool planar = false;
    size_t bpe = 0u;
    switch (fmt) {
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            bc = true;
            bpe = 8;
            break;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            bc = true;
            bpe = 16;
            break;

        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_YUY2:
            packed = true;
            bpe = 4;
            break;

        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
            packed = true;
            bpe = 8;
            break;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE:
            planar = true;
            bpe = 2;
            break;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
            planar = true;
            bpe = 4;
            break;
    }

    if (bc) {
        size_t num_blocks_wide = 0u;
        if (width > 0u) {
            num_blocks_wide = std::max<size_t>(1, (width + 3) / 4);
        }
        size_t num_blocks_high = 0u;
        if (height > 0) {
            num_blocks_high = std::max<size_t>(1, (height + 3) / 4);
        }
        row_bytes = num_blocks_wide * bpe;
        num_rows = num_blocks_high;
        num_bytes = row_bytes * num_blocks_high;
    }
    else if (packed) {
        row_bytes = ((width + 1) >> 1) * bpe;
        num_rows = height;
        num_bytes = row_bytes * height;
    }
    else if (fmt == DXGI_FORMAT_NV11) {
        row_bytes = ((width + 3) >> 2) * 4;
        num_rows = height * 2;  // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
        num_bytes = row_bytes * num_rows;
    }
    else if (planar) {
        row_bytes = ((width + 1) >> 1) * bpe;
        num_bytes = (row_bytes * height) + ((row_bytes * height + 1) >> 1);
        num_rows = height + ((height + 1) >> 1);
    }
    else {
        size_t bpp = DirectX::BitsPerPixel(fmt);
        row_bytes = (width * bpp + 7) / 8;
        num_rows = height;
        num_bytes = row_bytes * height;
    }

    if (out_num_bytes) {
        *out_num_bytes = num_bytes;
    }
    if (out_row_bytes) {
        *out_row_bytes = row_bytes;
    }
    if (out_num_rows) {
        *out_num_rows = num_rows;
    }
}