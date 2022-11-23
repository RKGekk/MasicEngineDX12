#include "scene_visitor_anim.h"

#include "scene_node.h"
#include "camera_node.h"
#include "mesh_node.h"
#include "aminated_mesh_node.h"
#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/effect_anim_pso.h"
#include "../graphics/directx12_wrappers/index_buffer.h"
#include "../graphics/directx12_wrappers/texture.h"
#include "../graphics/material.h"
#include "../graphics/mesh.h"

SceneVisitorAnim::SceneVisitorAnim(CommandList& command_list, std::shared_ptr<CameraNode> camera, EffectAnimPSO& pso, bool transparent, std::shared_ptr<Texture> shadow_map_texture) : m_command_list(command_list), m_camera(camera), m_lighting_anim_pso(pso), m_transparent_pass(transparent), m_shadow_map_texture(shadow_map_texture) {
	ResetCamera();
}

void SceneVisitorAnim::ResetCamera() {
	m_lighting_anim_pso.SetViewMatrix(m_camera->GetView());
	m_lighting_anim_pso.SetProjectionMatrix(m_camera->GetProjection());
}

void SceneVisitorAnim::ResetBoneTransforms(const std::vector<DirectX::XMFLOAT4X4>& final_transforms) {
	m_lighting_anim_pso.SetFinalBoneTransforms(final_transforms);
}

void SceneVisitorAnim::Visit(std::shared_ptr<SceneNode> scene_node) {
	std::shared_ptr<AnimatedMeshNode> pAnimMeshNode = std::dynamic_pointer_cast<AnimatedMeshNode>(scene_node);
	bool is_skin_node = !!pAnimMeshNode;

	if (is_skin_node) {
		if (pAnimMeshNode->GetIsInstanced()) return;

		auto world = scene_node->Get().ToRoot();
		m_lighting_anim_pso.SetWorldMatrix(world);
		m_lighting_anim_pso.SetFinalBoneTransforms(pAnimMeshNode->GetFinalTransformList());

		const auto& mesh_list = pAnimMeshNode->GetMeshes();
		for (const auto& mesh : mesh_list) {
			auto material = mesh->GetMaterial();
			if (material->IsTransparent() != m_transparent_pass) continue;

			m_lighting_anim_pso.SetMaterial(material);
			if (m_shadow_map_texture) {
				m_lighting_anim_pso.AddShadowTexture(m_shadow_map_texture);
			}

			m_lighting_anim_pso.Apply(m_command_list);

			m_command_list.SetPrimitiveTopology(mesh->GetPrimitiveTopology());

			const Mesh::BufferMap& buffer_map = mesh->GetVertexBuffers();
			for (auto vertex_buffer : buffer_map) {
				m_command_list.SetVertexBuffer(vertex_buffer.first, vertex_buffer.second);
			}

			size_t index_count = mesh->GetIndexCount();
			size_t vertex_count = mesh->GetVertexCount();
			size_t instance_count = 1u;
			size_t start_instance = 0u;
			if (index_count > 0u) {
				m_command_list.SetIndexBuffer(mesh->GetIndexBuffer());
				m_command_list.DrawIndexed(index_count, instance_count, 0u, 0u, start_instance);
			}
			else if (vertex_count > 0u) {
				m_command_list.Draw(vertex_count, instance_count, 0u, start_instance);
			}
		}
	}
}