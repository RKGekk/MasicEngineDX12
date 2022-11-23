#include "screen_element_scene.h"

#include "engine.h"
#include "../actors/transform_component.h"
#include "../graphics/d3d12_renderer.h"
#include "../graphics/i_renderer.h"
#include "../graphics/directx12_wrappers/command_queue.h"
#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/directx12_wrappers/swap_chain.h"
#include "../graphics/directx12_wrappers/texture.h"
#include "../graphics/directx12_wrappers/resource_state_tracker.h"
#include "../nodes/scene_visitor.h"
#include "../nodes/scene_visitor_anim.h"
#include "../nodes/shadow_scene_visitor.h"
#include "../nodes/shadow_scene_anim_visitor.h"
#include "../nodes/qualifier_node.h"
#include "../nodes/light_manager.h"
#include "../nodes/mesh_manager.h"
#include "../nodes/skinned_mesh_manager.h"
#include "../nodes/shadow_manager.h"
#include "../nodes/mesh_node.h"
#include "../nodes/aminated_mesh_node.h"
#include "../nodes/camera_node.h"
#include "../nodes/basic_camera_node.h"
#include "../nodes/shadow_camera_node.h"
#include "../events/evt_data_modified_scene_component.h"

ScreenElementScene::ScreenElementScene() : Scene() {
	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());
	std::shared_ptr<Device> device = renderer->GetDevice();

	m_back_buffer_format = renderer->GetBackBufferFormat();
	m_width = renderer->GetRenderTargetWidth();
	m_height = renderer->GetRenderTargetHeight();
	//DXGI_SAMPLE_DESC sample_desc = device->GetMultisampleQualityLevels(m_back_buffer_format);
	DXGI_SAMPLE_DESC sample_desc = { 1, 0 };
	auto color_desc = CD3DX12_RESOURCE_DESC::Tex2D(m_back_buffer_format, m_width, m_height, 1, 1, sample_desc.Count, sample_desc.Quality, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	D3D12_CLEAR_VALUE color_clear_value = {};
	color_clear_value.Format = color_desc.Format;
	color_clear_value.Color[0] = 0.4f;
	color_clear_value.Color[1] = 0.6f;
	color_clear_value.Color[2] = 0.9f;
	color_clear_value.Color[3] = 1.0f;

	auto color_texture = device->CreateTexture(color_desc, &color_clear_value);
	color_texture->SetName(L"Color Render Target");

	m_depth_buffer_format = DXGI_FORMAT_D32_FLOAT;
	auto depth_desc = CD3DX12_RESOURCE_DESC::Tex2D(m_depth_buffer_format, m_width, m_height, 1, 1, sample_desc.Count, sample_desc.Quality, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	D3D12_CLEAR_VALUE depth_clear_value = {};
	depth_clear_value.Format = depth_desc.Format;
	depth_clear_value.DepthStencil = { 1.0f, 0 };

	auto depth_texture = device->CreateTexture(depth_desc, &depth_clear_value);
	depth_texture->SetName(L"Depth Render Target");

	m_render_target.AttachTexture(AttachmentPoint::Color0, color_texture);
	m_render_target.AttachTexture(AttachmentPoint::DepthStencil, depth_texture);

	m_lighting_pso = std::make_shared<EffectPSO>(device, true, false);
	m_lighting_anim_pso = std::make_shared<EffectAnimPSO>(device, true, false);
	m_lighting_instanced_pso = std::make_shared<EffectInstancedPSO>(device);
	m_lighting_anim_instanced_pso = std::make_shared<EffectAnimInstancedPSO>(device);

	m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height));
	m_scissor_rect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);

	RegisterAllDelegates();
}

ScreenElementScene::~ScreenElementScene() {
	RemoveAllDelegates();
}

void ScreenElementScene::VOnUpdate(const GameTimerDelta& delta) {
	OnUpdate();
}

HRESULT ScreenElementScene::VOnRestore() {
	OnRestore();
	return S_OK;
}

HRESULT ScreenElementScene::VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList> command_list) {
	std::shared_ptr<SceneNode> root_scene_node = GetRootNode()->GetNodesGroup(0);
	if(!root_scene_node) return S_OK;

	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());
	std::shared_ptr<Device> device = renderer->GetDevice();

	std::shared_ptr<HumanView> human_view = engine->GetGameLogic()->GetHumanView();
	std::shared_ptr<BasicCameraNode> camera = human_view->VGetCamera();

	//DirectX::XMFLOAT3 cam_pos = camera->Get().Position3();
	//DirectX::XMFLOAT3 cam_pos = camera->Get().CumulativePosition3();
	//cam_pos.x += delta.fGetDeltaSeconds() * 0.5f;
	//if (cam_pos.x > 3.0f) cam_pos.x -= 3.0f;
	//camera->SetPosition3(cam_pos);
	//camera->GetParent()->SetPosition3(cam_pos);

	//DirectX::XMFLOAT4X4 cam_trans = camera->Get().CumulativeToWorld4x4();
	//cam_trans.m[3][0] += delta.fGetDeltaSeconds() * 0.5f;
	//camera->SetTransform4x4(&cam_trans, nullptr);
	//camera->GetParent()->SetTransform4x4(&cam_trans, nullptr);

	/*WeakActorPtr weak_camera_actor = engine->GetGameLogic()->VGetActorByName("camera1");
	if (auto cam_actor = weak_camera_actor.lock()) {
		std::shared_ptr<TransformComponent> tc = cam_actor->GetComponent<TransformComponent>().lock();
		DirectX::XMFLOAT3 cam_pos = tc->GetPosition3f();
		cam_pos.x += delta.fGetDeltaSeconds() * 0.1f;
		if (cam_pos.x > 0.0f) cam_pos.x -= 2.0f;
		tc->SetPosition3f(cam_pos);
	}*/

	m_mesh_manager->CalcInstances(*camera);
	m_skinned_mesh_manager->CalcInstances(*camera);
	m_light_manager->CalcLighting(camera->GetView());

	std::shared_ptr<ShadowCameraNode> shadow_camera = m_shadow_manager->GetShadow();
	if (shadow_camera) {
		const ShadowCameraNode::ShadowCameraProps& shadow_camera_props = shadow_camera->GetShadowProps();

		if(!m_shadow_pso) m_shadow_pso = std::make_shared<EffectShadowPSO>(device, m_shadow_manager);
		if(!m_shadow_anim_pso) m_shadow_anim_pso = std::make_shared<EffectShadowAnimPSO>(device, m_shadow_manager);
		if(!m_shadow_instanced_pso) m_shadow_instanced_pso = std::make_shared<EffectShadowInstancedPSO>(device, m_shadow_manager);
		if(!m_shadow_anim_instanced_pso) m_shadow_anim_instanced_pso = std::make_shared<EffectAnimShadowInstancedPSO>(device, m_shadow_manager);

		m_shadow_instanced_pso->SetMeshManager(m_mesh_manager);
		m_shadow_anim_instanced_pso->SetSkinnedMeshManager(m_skinned_mesh_manager);
		m_shadow_instanced_pso->SetViewMatrix(*shadow_camera);
		m_shadow_anim_instanced_pso->SetViewMatrix(*shadow_camera);
		m_shadow_instanced_pso->SetRenderTargetSize({ (float)shadow_camera_props.ShadowMapWidth, (float)shadow_camera_props.ShadowMapHeight });
		m_shadow_anim_instanced_pso->SetRenderTargetSize({ (float)shadow_camera_props.ShadowMapWidth, (float)shadow_camera_props.ShadowMapHeight });

		ShadowSceneVisitor shadow_pass(*command_list, shadow_camera, *m_shadow_pso, false);
		ShadowSceneAnimVisitor shadow_anim_pass(*command_list, shadow_camera, *m_shadow_anim_pso, false);

		std::shared_ptr<Texture> shadow_target_depth = m_shadow_manager->GetShadowMapTexture();
		command_list->ClearDepthStencilTexture(shadow_target_depth, D3D12_CLEAR_FLAG_DEPTH);

		command_list->SetViewport(m_shadow_manager->GetShadowViewport());
		command_list->SetScissorRect(m_shadow_manager->GetShadowRect());
		command_list->SetRenderTarget(*m_shadow_manager->GetRT());

		root_scene_node->Accept(shadow_pass);
		root_scene_node->Accept(shadow_anim_pass);
		m_shadow_instanced_pso->Apply(*command_list, delta);
		m_shadow_anim_instanced_pso->Apply(*command_list, delta);

		//m_lighting_pso->SetShadowMatrix(shadow_camera->GetShadowTranform());
		////m_lighting_pso->SetShadowMatrix(DirectX::XMMatrixTranspose(shadow_camera->GetShadowTranform()));
		//m_lighting_anim_pso->SetShadowMatrix(shadow_camera->GetShadowTranform());
		////m_shadow_manager->GetShadowMapTexture()->
		m_lighting_pso->SetShadowMatrix(DirectX::XMMatrixTranspose(shadow_camera->GetShadowTranform()));
		m_lighting_anim_pso->SetShadowMatrix(DirectX::XMMatrixTranspose(shadow_camera->GetShadowTranform()));
		
		if (!m_shadow_map_texture) {
			//DXGI_SAMPLE_DESC sample_desc = device->GetMultisampleQualityLevels(m_back_buffer_format);
			DXGI_SAMPLE_DESC sample_desc = { 1, 0 };
			//auto depth_desc = CD3DX12_RESOURCE_DESC::Tex2D(m_shadow_manager->GetShadowBufferFormat(), shadow_camera_props.ShadowMapWidth, shadow_camera_props.ShadowMapHeight, 1, 1, sample_desc.Count, sample_desc.Quality);
			auto depth_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_FLOAT, shadow_camera_props.ShadowMapWidth, shadow_camera_props.ShadowMapHeight, 1, 1, sample_desc.Count, sample_desc.Quality);
			//D3D12_CLEAR_VALUE shadow_clear_value = {};
			//shadow_clear_value.Format = depth_desc.Format;
			//shadow_clear_value.DepthStencil = { 1.0f, 0 };

			//m_shadow_map_texture = device->CreateTexture(depth_desc, &shadow_clear_value);
			m_shadow_map_texture = device->CreateTexture(depth_desc);
			m_shadow_map_texture->SetName(L"Shadow Texture");

			//ResourceStateTracker::AddGlobalResourceState(m_shadow_map_texture->GetD3D12Resource().Get(), D3D12_RESOURCE_STATE_COMMON);
		}
		command_list->CopyResource(m_shadow_map_texture, m_shadow_manager->GetShadowMapTexture());
		m_lighting_instanced_pso->SetShadowMapTexture(m_shadow_map_texture);
		m_lighting_anim_instanced_pso->SetShadowMapTexture(m_shadow_map_texture);
	}

	EffectPSO::FogProperties fog_props = {};
	fog_props.FogColor = m_scene_config.FogColor;
	fog_props.FogRange = m_scene_config.FogRange;
	fog_props.FogStart = m_scene_config.FogStart;

	EffectInstancedPSO::FogProperties fog_inst_props = {};
	fog_inst_props.FogColor = m_scene_config.FogColor;
	fog_inst_props.FogRange = m_scene_config.FogRange;
	fog_inst_props.FogStart = m_scene_config.FogStart;

	EffectAnimInstancedPSO::FogProperties fog_anim_inst_props = {};
	fog_anim_inst_props.FogColor = m_scene_config.FogColor;
	fog_anim_inst_props.FogRange = m_scene_config.FogRange;
	fog_anim_inst_props.FogStart = m_scene_config.FogStart;

	EffectAnimPSO::FogProperties fog_anim_props = {};
	fog_anim_props.FogColor = m_scene_config.FogColor;
	fog_anim_props.FogRange = m_scene_config.FogRange;
	fog_anim_props.FogStart = m_scene_config.FogStart;

	m_lighting_pso->SetFogProperties(fog_props);
	m_lighting_pso->SetLightManager(m_light_manager);
	m_lighting_anim_pso->SetFogProperties(fog_anim_props);
	m_lighting_anim_pso->SetLightManager(m_light_manager);
	m_lighting_instanced_pso->SetFogProperties(fog_inst_props);
	m_lighting_anim_instanced_pso->SetFogProperties(fog_anim_inst_props);
	m_lighting_instanced_pso->SetLightManager(m_light_manager);
	m_lighting_anim_instanced_pso->SetLightManager(m_light_manager);
	m_lighting_instanced_pso->SetShadowManager(m_shadow_manager);
	m_lighting_anim_instanced_pso->SetShadowManager(m_shadow_manager);
	m_lighting_instanced_pso->SetMeshManager(m_mesh_manager);
	m_lighting_anim_instanced_pso->SetSkinnedMeshManager(m_skinned_mesh_manager);
	m_lighting_instanced_pso->SetViewMatrix(*camera);
	m_lighting_anim_instanced_pso->SetViewMatrix(*camera);
	m_lighting_instanced_pso->SetRenderTargetSize({ (float)m_width, (float)m_height });
	m_lighting_anim_instanced_pso->SetRenderTargetSize({ (float)m_width, (float)m_height });

	SceneVisitor opaque_pass(*command_list, camera, *m_lighting_pso, false, m_shadow_map_texture);
	SceneVisitorAnim opaque_anim_pass(*command_list, camera, *m_lighting_anim_pso, false, m_shadow_map_texture);

	//FLOAT clear_color[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	//FLOAT clear_color[] = { 0.729412f, 0.72549f, 0.705882f, 1.0f };
	DirectX::XMFLOAT4 cc = renderer->VGetClearColor4f();

	std::shared_ptr<Texture> render_target_color = m_render_target.GetTexture(AttachmentPoint::Color0);
	std::shared_ptr<Texture> render_target_depth = m_render_target.GetTexture(AttachmentPoint::DepthStencil);
	
	//command_list->ClearTexture(render_target_color, clear_color);
	command_list->ClearTexture(render_target_color, &cc.x);
	command_list->ClearDepthStencilTexture(render_target_depth, D3D12_CLEAR_FLAG_DEPTH);

	command_list->SetViewport(m_viewport);
	command_list->SetScissorRect(m_scissor_rect);
	command_list->SetRenderTarget(m_render_target);

	root_scene_node->Accept(opaque_pass);
	root_scene_node->Accept(opaque_anim_pass);
	m_lighting_instanced_pso->Apply(*command_list, delta);
	m_lighting_anim_instanced_pso->Apply(*command_list, delta);

	auto swap_chain_back_buffer_color = renderer->GetRenderTarget().GetTexture(AttachmentPoint::Color0);
	auto swap_chain_back_buffer_depth = renderer->GetRenderTarget().GetTexture(AttachmentPoint::DepthStencil);
	if (render_target_color->GetD3D12ResourceDesc().SampleDesc.Count > 1) {
		command_list->ResolveSubresource(swap_chain_back_buffer_color, render_target_color);
	}
	else {
		command_list->CopyResource(swap_chain_back_buffer_color, render_target_color);
	}
	command_list->CopyResource(swap_chain_back_buffer_depth, render_target_depth);

	return S_OK;
}

HRESULT ScreenElementScene::VOnLostDevice() {
	OnLostDevice();
	return S_OK;
}

int ScreenElementScene::VGetZOrder() const {
	return 0;
}

void ScreenElementScene::VSetZOrder(int const zOrder) {}

LRESULT ScreenElementScene::VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) {
	return 0;
}

bool ScreenElementScene::VIsVisible() const {
	return m_is_visible;
}

void ScreenElementScene::VSetVisible(bool visible) {
	m_is_visible = visible;
}

bool ScreenElementScene::VAddChild(std::shared_ptr<SceneNode> kid) {
	return Scene::AddChild(kid);
}

void ScreenElementScene::ModifiedSceneNodeComponentDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Modified_Scene_Component> pCastEventData = std::static_pointer_cast<EvtData_Modified_Scene_Component>(pEventData);
	std::shared_ptr<SceneNode> node = pCastEventData->GetSceneNode().lock();

	ModifiedSceneNode(node);
}

void ScreenElementScene::ModifiedSceneNode(std::shared_ptr<SceneNode> node) {
	if (std::shared_ptr<MeshNode> pMesh = std::dynamic_pointer_cast<MeshNode>(node)) {
		if (pMesh->GetIsInstanced() && m_mesh_manager->GetMeshCount(pMesh)) {
			m_mesh_manager->UpdateInstancesBuffer();
		}
	};

	if (std::shared_ptr<AnimatedMeshNode> pAnimMesh = std::dynamic_pointer_cast<AnimatedMeshNode>(node)) {
		if (pAnimMesh->GetIsInstanced() && m_skinned_mesh_manager->GetMeshCount(pAnimMesh)) {
			//m_skinned_mesh_manager->UpdateInstancesBuffer();
			m_skinned_mesh_manager->UpdateInstancesBuffer(pAnimMesh->Get().Name());
		}
	};

	for (const auto& child_node : node->VGetChildren()) {
		ModifiedSceneNode(child_node);
	}
}

void ScreenElementScene::RegisterAllDelegates() {
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VAddListener({ connect_arg<&ScreenElementScene::ModifiedSceneNodeComponentDelegate>, this }, EvtData_Modified_Scene_Component::sk_EventType);
}

void ScreenElementScene::RemoveAllDelegates() {
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VRemoveListener({ connect_arg<&ScreenElementScene::ModifiedSceneNodeComponentDelegate>, this }, EvtData_Modified_Scene_Component::sk_EventType);
}
