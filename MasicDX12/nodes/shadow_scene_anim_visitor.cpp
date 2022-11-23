#include "shadow_scene_anim_visitor.h"

#include "scene_node.h"
#include "camera_node.h"
#include "shadow_camera_node.h"
#include "mesh_node.h"
#include "aminated_mesh_node.h"
#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/effect_shadow_anim_pso.h"
#include "../graphics/directx12_wrappers/index_buffer.h"
#include "../graphics/material.h"
#include "../graphics/mesh.h"

ShadowSceneAnimVisitor::ShadowSceneAnimVisitor(CommandList& command_list, std::shared_ptr<CameraNode> camera, EffectShadowAnimPSO& pso, bool transparent) : m_command_list(command_list), m_camera(camera), m_shadow_pso(pso), m_transparent_pass(transparent) {
	ResetCamera();
}

void ShadowSceneAnimVisitor::ResetCamera() {
	m_shadow_pso.SetViewMatrix(m_camera->GetView());
	m_shadow_pso.SetProjectionMatrix(m_camera->GetProjection());
}

void ShadowSceneAnimVisitor::ResetBoneTransforms(const std::vector<DirectX::XMFLOAT4X4>& final_transforms) {
	m_shadow_pso.SetFinalBoneTransforms(final_transforms);
}

void ShadowSceneAnimVisitor::Visit(std::shared_ptr<SceneNode> scene_node) {
	std::shared_ptr<AnimatedMeshNode> pAnimMeshNode = std::dynamic_pointer_cast<AnimatedMeshNode>(scene_node);
	bool is_skin_node = !!pAnimMeshNode;

	if (is_skin_node) {
		if (pAnimMeshNode->GetIsInstanced()) return;

		auto world = scene_node->Get().ToRoot();
		m_shadow_pso.SetWorldMatrix(world);
		m_shadow_pso.SetFinalBoneTransforms(pAnimMeshNode->GetFinalTransformList());

		const MeshNode::MeshList& mesh_list = pAnimMeshNode->GetMeshes();
		for (const auto& mesh : mesh_list) {
			m_shadow_pso.Apply(m_command_list);

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

