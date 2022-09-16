#include "actor_menu_ui.h"

#include "engine.h"
#include "../actors/actor_component.h"
#include "../actors/transform_component.h"
#include "../actors/particle_component.h"
#include "../graphics/d3d12_renderer.h"
#include "../graphics/i_renderer.h"
#include "../graphics/directx12_wrappers/command_queue.h"
#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/directx12_wrappers/swap_chain.h"
#include "../graphics/directx12_wrappers/texture.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void GetSurfaceInfo(_In_ size_t width, _In_ size_t height, _In_ DXGI_FORMAT fmt, size_t* out_num_bytes, _Out_opt_ size_t* out_row_bytes, _Out_opt_ size_t* out_num_rows);

enum RootParameters {
	MatrixCB,
	FontTexture,
	NumRootParameters
};

ActorMenuUI::ActorMenuUI(std::shared_ptr<ProcessManager> pm) : m_hwnd(nullptr), m_pImgui_ctx(nullptr) {
	using namespace std::literals;

	m_show_menu = true;
	m_actor_id = 0;
	m_hwnd = Engine::GetEngine()->GetRenderer()->GetRenderWindow()->GetHWND();
	m_pImgui_ctx = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_pImgui_ctx);
	if (!ImGui_ImplWin32_Init(m_hwnd)) {
		throw std::exception("Failed to initialize ImGui");
	}
	ImGuiIO& io = ImGui::GetIO();

	io.FontGlobalScale = ::GetDpiForWindow(m_hwnd) / 96.0f;
	io.FontAllowUserScaling = true;

	unsigned char* pixel_data = nullptr;
	int width;
	int height;
	io.Fonts->GetTexDataAsRGBA32(&pixel_data, &width, &height);

	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());
	std::shared_ptr<Device> device = renderer->GetDevice();
	std::shared_ptr<SwapChain> swap_chain = renderer->GetSwapChain();
	CommandQueue& command_queue = device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	std::shared_ptr<CommandList> command_list = command_queue.GetCommandList();

	auto font_texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

	m_font_texture = device->CreateTexture(font_texture_desc);
	m_font_texture->SetName(L"ImGui Font Texture");
	m_font_srv = device->CreateShaderResourceView(m_font_texture);

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

	auto d3d12_device = device->GetD3D12Device();

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

	m_root_signature = device->CreateRootSignature("ActorMenuUIRootSign", root_signature_description);

	const D3D12_INPUT_ELEMENT_DESC input_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(ImDrawVert, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(ImDrawVert, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(ImDrawVert, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	const D3D12_INPUT_LAYOUT_DESC input_layout_desc = {input_layout, 3};

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

	DXGI_SAMPLE_DESC sample_desc = { 1, 0 };

	Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader_blob;
	HRESULT hr = D3DReadFileToBlob(L"ImGUI_VS.cso", vertex_shader_blob.GetAddressOf());
	ThrowIfFailed(hr);
	m_vertex_shader = std::make_shared<VertexShader>(vertex_shader_blob, "main"s, "ImGUI_VS.cso"s);

	Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader_blob;
	hr = D3DReadFileToBlob(L"ImGUI_PS.cso", pixel_shader_blob.GetAddressOf());
	ThrowIfFailed(hr);
	m_pixel_shader = std::make_shared<PixelShader>(pixel_shader_blob, "main"s, "ImGUI_PS.cso"s);

	m_vertex_shader->AddRegister({ 0, 0, ShaderRegister::ConstantBuffer }, "vertexBuffer"s);
	m_vertex_shader->SetInputAssemblerLayout(input_layout_desc);
	m_vertex_shader->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	m_pixel_shader->AddRegister({ 0, 0, ShaderRegister::Sampler }, "sampler0"s);
	m_pixel_shader->AddRegister({ 0, 0, ShaderRegister::ShaderResource }, "texture0"s);
	m_pixel_shader->SetRenderTargetFormat(AttachmentPoint::Color0, renderer->GetBackBufferFormat());
	m_pixel_shader->SetRenderTargetFormat(AttachmentPoint::DepthStencil, DXGI_FORMAT_D32_FLOAT);
	m_pixel_shader->SetSample(sample_desc);
	m_pixel_shader->SetBlendState(CD3DX12_BLEND_DESC(blend_desc));
	m_pixel_shader->SetRasterizerState(CD3DX12_RASTERIZER_DESC(rasterizer_desc));
	m_pixel_shader->SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(depth_stencil_desc));

	m_pipeline_state = device->CreateGraphicsPipelineState("PSOForImGUI"s, m_root_signature, m_vertex_shader, m_pixel_shader);

	Set(pm);
}

ActorMenuUI::~ActorMenuUI() {
	ImGui::EndFrame();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext(m_pImgui_ctx);
	m_pImgui_ctx = nullptr;
}

HRESULT ActorMenuUI::VOnRestore() {
	return S_OK;
}

HRESULT ActorMenuUI::VOnRender(const GameTimerDelta& delta) {
	if (!m_show_menu) { return S_OK; }

	ImGui::SetCurrentContext(m_pImgui_ctx);
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Actor Menu");

	if (ImGui::CollapsingHeader("Actors")) {
		ImGui::Text(m_actor_name.c_str());

		if (ImGui::InputInt("Actor ID", &m_actor_id)) {
			std::shared_ptr<Actor> act = Engine::GetEngine()->GetGameLogic()->VGetActor(m_actor_id).lock();
			if (act) {
				m_actor_name = act->GetName();

				std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>().lock();
				if (tc) {
					m_transform_exists = true;
					m_transform = tc->GetTransform4x4f();
					m_yaw_pith_roll = tc->GetYawPitchRoll3f();
					m_yaw_pith_roll.x = DirectX::XMConvertToDegrees(m_yaw_pith_roll.x);
					m_yaw_pith_roll.y = DirectX::XMConvertToDegrees(m_yaw_pith_roll.y);
					m_yaw_pith_roll.z = DirectX::XMConvertToDegrees(m_yaw_pith_roll.z);
					m_scale = tc->GetScale3f();
				}
				else {
					m_transform_exists = false;
					DirectX::XMStoreFloat4x4(&m_transform, DirectX::XMMatrixIdentity());
					m_yaw_pith_roll = { 0.0f, 0.0f, 0.0f };
					m_scale = { 1.0f, 1.0f, 1.0f };
				}

				std::shared_ptr<ParticleComponent> pc = act->GetComponent<ParticleComponent>().lock();
				if (pc) {
					m_particle_exists = true;
					Particle& p = pc->VGetParticle();
					m_damping = p.getDamping();
					m_radius = p.getRadius();
					m_mass = p.getMass();
				}
				else {
					m_particle_exists = false;
					m_damping = 1.0f;
					m_radius = 1.0f;
					m_mass = 1.0f;
				}
			}
			else {
				m_transform_exists = false;
				DirectX::XMStoreFloat4x4(&m_transform, DirectX::XMMatrixIdentity());
			}
		}
		else {
			std::shared_ptr<Actor> act = Engine::GetEngine()->GetGameLogic()->VGetActor(m_actor_id).lock();
			if (act) {
				std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>().lock();
				if (tc) {
					m_transform = tc->GetTransform4x4f();
				}

				if (m_transform_exists && ImGui::SliderFloat4("Transform row 1", ((float*)&m_transform) + 0, -8.0f, 8.0f)) {}
				if (m_transform_exists && ImGui::SliderFloat4("Transform row 2", ((float*)&m_transform) + 4, -8.0f, 8.0f)) {}
				if (m_transform_exists && ImGui::SliderFloat4("Transform row 3", ((float*)&m_transform) + 8, -8.0f, 8.0f)) {}
				if (m_transform_exists && ImGui::SliderFloat4("Transform row 4", ((float*)&m_transform) + 12, -8.0f, 8.0f)) {
					std::shared_ptr<ParticleComponent> pc = act->GetComponent<ParticleComponent>().lock();
					if (pc) {
						m_particle_exists = true;
						m_transform_exists = true;
						pc->VGetParticle().setPosition(m_transform._41, m_transform._42, m_transform._43);
					}
					else if (tc) {
						m_transform_exists = true;
						tc->SetTransform(m_transform);
					}

				}
				if (m_transform_exists && ImGui::SliderFloat3("Scale", ((float*)&m_scale), 0.0f, 3.0f)) {
					if (tc) {
						m_transform_exists = true;
						tc->SetScale3f(m_scale);
					}
				}
				if (m_transform_exists && ImGui::SliderFloat3("Yaw Pith Roll", ((float*)&m_yaw_pith_roll), -180.0f, 180.0f)) {
					if (tc) {
						m_transform_exists = true;
						tc->SetYawPitchRollDeg3f(m_yaw_pith_roll);
					}
				}
				if (m_particle_exists && ImGui::SliderFloat("Damping", ((float*)&m_damping), 0.0f, 1.0f)) {
					m_particle_exists = true;
					std::shared_ptr<ParticleComponent> pc = act->GetComponent<ParticleComponent>().lock();
					if (pc) {
						m_particle_exists = true;
						pc->VGetParticle().setDamping(m_damping);
					}
				}
				if (m_particle_exists && ImGui::SliderFloat("Radius", ((float*)&m_radius), 0.0f, 1.0f)) {
					m_particle_exists = true;
					std::shared_ptr<ParticleComponent> pc = act->GetComponent<ParticleComponent>().lock();
					if (pc) {
						m_particle_exists = true;
						pc->VGetParticle().setRadius(m_radius);
					}
				}
				if (m_particle_exists && ImGui::SliderFloat("Mass", ((float*)&m_mass), 0.0f, 1000.0f)) {
					m_particle_exists = true;
					std::shared_ptr<ParticleComponent> pc = act->GetComponent<ParticleComponent>().lock();
					if (pc) {
						m_particle_exists = true;
						pc->VGetParticle().setMass(m_mass);
					}
				}
			}
		}
	}

	ImGui::End();

	ImGui::SetCurrentContext(m_pImgui_ctx);
	ImGui::Render();

	ImGuiIO& io = ImGui::GetIO();
	ImDrawData* draw_data = ImGui::GetDrawData();

	if (!draw_data || draw_data->CmdListsCount == 0) return S_OK;;

	ImVec2 display_pos = draw_data->DisplayPos;

	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());
	std::shared_ptr<Device> device = renderer->GetDevice();
	std::shared_ptr<SwapChain> swap_chain = renderer->GetSwapChain();
	CommandQueue& command_queue = device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	std::shared_ptr<CommandList> command_list = command_queue.GetCommandList();

	command_list->SetPipelineState(m_pipeline_state);
	command_list->SetGraphicsRootSignature(m_root_signature);
	command_list->SetRenderTarget(swap_chain->GetRenderTarget());

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

	return S_OK;
}

void ActorMenuUI::VOnUpdate(const GameTimerDelta& delta) {}

int ActorMenuUI::VGetZOrder() const {
	return 1;
}

void ActorMenuUI::VSetZOrder(int const zOrder) {}

LRESULT ActorMenuUI::VOnMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) { return true; }
	return 0;
}

void ActorMenuUI::Set(std::shared_ptr<ProcessManager>) {
	if (m_actor_id == 0) {
		m_transform_exists = false;
		m_particle_exists = false;
		DirectX::XMStoreFloat4x4(&m_transform, DirectX::XMMatrixIdentity());
		return;
	}
	std::shared_ptr<Actor> act = Engine::GetEngine()->GetGameLogic()->VGetActor(m_actor_id).lock();
	if (act) {
		m_actor_name = act->GetName();

		std::shared_ptr<TransformComponent> rc = act->GetComponent<TransformComponent>().lock();
		if (rc) {
			m_transform_exists = true;
			m_transform = rc->GetTransform4x4f();
		}
	}
}
