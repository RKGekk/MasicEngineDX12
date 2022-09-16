#include "actor_menu_ui.h"

#include "engine.h"
#include "../actors/actor_component.h"
#include "../actors/transform_component.h"
#include "../actors/particle_component.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ActorMenuUI::ActorMenuUI(std::shared_ptr<ProcessManager> pm) : m_hwnd(nullptr), m_pImgui_ctx(nullptr) {
	m_show_menu = true;
	m_actor_id = 0;
	m_hwnd = Engine::GetEngine()->GetRenderer()->GetRenderWindow()->GetHWND();
	m_pImgui_ctx = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_pImgui_ctx);
	if (!ImGui_ImplWin32_Init(m_hwnd)) {
		throw std::exception("Failed to initialize ImGui");
	}
	//ImGuiIO& io = ImGui::GetIO();

	//io.FontGlobalScale = ::GetDpiForWindow(m_hWnd) / 96.0f;
	//io.FontAllowUserScaling = true;

	//unsigned char* pixel_data = nullptr;
	//int width;
	//int height;
	//io.Fonts->GetTexDataAsRGBA32(&pixel_data, &width, &height);

	//auto& command_queue = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	//auto command_list = command_queue.GetCommandList();

	//auto font_texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

	//m_font_texture = m_device.CreateTexture(font_texture_desc);
	//m_font_texture->SetName(L"ImGui Font Texture");
	//m_font_srv = m_device.CreateShaderResourceView(m_font_texture);

	//size_t row_pitch;
	//size_t slice_pitch;
	//GetSurfaceInfo(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, &slice_pitch, &row_pitch, nullptr);

	//D3D12_SUBRESOURCE_DATA subresource_data;
	//subresource_data.pData = pixel_data;
	//subresource_data.RowPitch = row_pitch;
	//subresource_data.SlicePitch = slice_pitch;

	//command_list->CopyTextureSubresource(m_font_texture, 0, 1, &subresource_data);
	//command_list->GenerateMips(m_font_texture);

	//command_queue.ExecuteCommandList(command_list);

	//auto d3d12_device = m_device.GetD3D12Device();

	//D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	//CD3DX12_DESCRIPTOR_RANGE1 descriptor_rage(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	//CD3DX12_ROOT_PARAMETER1 root_parameters[RootParameters::NumRootParameters];
	//root_parameters[RootParameters::MatrixCB].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
	//root_parameters[RootParameters::FontTexture].InitAsDescriptorTable(1, &descriptor_rage, D3D12_SHADER_VISIBILITY_PIXEL);

	//CD3DX12_STATIC_SAMPLER_DESC linear_repeat_sampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT);
	//linear_repeat_sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	//linear_repeat_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description;
	//root_signature_description.Init_1_1(RootParameters::NumRootParameters, root_parameters, 1, &linear_repeat_sampler, root_signature_flags);

	//m_root_signature = m_device.CreateRootSignature(root_signature_description.Desc_1_1);

	//const D3D12_INPUT_ELEMENT_DESC input_layout[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(ImDrawVert, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(ImDrawVert, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(ImDrawVert, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//};

	//D3D12_BLEND_DESC blend_desc = {};
	//blend_desc.RenderTarget[0].BlendEnable = true;
	//blend_desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blend_desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//blend_desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//blend_desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	//blend_desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//blend_desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//D3D12_RASTERIZER_DESC rasterizer_desc = {};
	//rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
	//rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
	//rasterizer_desc.FrontCounterClockwise = FALSE;
	//rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	//rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	//rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	//rasterizer_desc.DepthClipEnable = true;
	//rasterizer_desc.MultisampleEnable = FALSE;
	//rasterizer_desc.AntialiasedLineEnable = FALSE;
	//rasterizer_desc.ForcedSampleCount = 0;
	//rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	//depth_stencil_desc.DepthEnable = false;
	//depth_stencil_desc.StencilEnable = false;

	//struct PipelineStateStream {
	//	CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
	//	CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
	//	CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
	//	CD3DX12_PIPELINE_STATE_STREAM_VS VS;
	//	CD3DX12_PIPELINE_STATE_STREAM_PS PS;
	//	CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	//	CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC SampleDesc;
	//	CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
	//	CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER RasterizerState;
	//	CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
	//} pipeline_state_stream;

	//static Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader_blob;
	//if (!vertex_shader_blob) {
	//	HRESULT hr = D3DReadFileToBlob(L"ImGUI_VS.cso", vertex_shader_blob.GetAddressOf());
	//	ThrowIfFailed(hr);
	//}

	//static Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader_blob;
	//if (!pixel_shader_blob) {
	//	HRESULT hr = D3DReadFileToBlob(L"ImGUI_PS.cso", pixel_shader_blob.GetAddressOf());
	//	ThrowIfFailed(hr);
	//}

	//pipeline_state_stream.pRootSignature = m_root_signature->GetD3D12RootSignature().Get();
	//pipeline_state_stream.InputLayout = { input_layout, 3 };
	//pipeline_state_stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//pipeline_state_stream.VS = CD3DX12_SHADER_BYTECODE(vertex_shader_blob.Get());
	//pipeline_state_stream.PS = CD3DX12_SHADER_BYTECODE(pixel_shader_blob.Get());
	//pipeline_state_stream.RTVFormats = render_target.GetRenderTargetFormats();
	//pipeline_state_stream.SampleDesc = render_target.GetSampleDesc();
	//pipeline_state_stream.BlendDesc = CD3DX12_BLEND_DESC(blend_desc);
	//pipeline_state_stream.RasterizerState = CD3DX12_RASTERIZER_DESC(rasterizer_desc);
	//pipeline_state_stream.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(depth_stencil_desc);

	//m_pipeline_state = device.CreatePipelineStateObject(pipeline_state_stream);

	Set(pm);
}

ActorMenuUI::~ActorMenuUI() {}

HRESULT ActorMenuUI::VOnRestore() {
	return S_OK;
}

HRESULT ActorMenuUI::VOnRender(const GameTimerDelta& delta) {
	if (!m_show_menu) { return S_OK; }

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
