#include "scene_visitor.h"

#include "scene_node.h"
#include "camera_node.h"
#include "mesh_node.h"
#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/effect_pso.h"
#include "../graphics/directx12_wrappers/index_buffer.h"
#include "../graphics/directx12_wrappers/texture.h"
#include "../graphics/material.h"
#include "../graphics/mesh.h"

SceneVisitor::SceneVisitor(CommandList& command_list, std::shared_ptr<CameraNode> camera, EffectPSO& pso, bool transparent, std::shared_ptr<Texture> shadow_map_texture) : m_command_list(command_list), m_camera(camera), m_lighting_pso(pso), m_transparent_pass(transparent), m_shadow_map_texture(shadow_map_texture) {
	ResetCamera();
}

void SceneVisitor::ResetCamera() {
	m_lighting_pso.SetViewMatrix(m_camera->GetView());
	m_lighting_pso.SetProjectionMatrix(m_camera->GetProjection());
}

void SceneVisitor::Visit(std::shared_ptr<SceneNode> scene_node) {
	std::shared_ptr<MeshNode> pMeshNode = std::dynamic_pointer_cast<MeshNode>(scene_node);
	if (!pMeshNode) return;
	if (pMeshNode->GetIsInstanced()) return;

	//auto world = scene_node->Get().CumulativeToWorld();
	auto world = scene_node->Get().FullCumulativeToWorld();
	m_lighting_pso.SetWorldMatrix(world);

	const MeshNode::MeshList& mesh_list = pMeshNode->GetMeshes();
	for (const auto& mesh : mesh_list) {
		auto material = mesh->GetMaterial();
		if (material->IsTransparent() != m_transparent_pass) continue;
		
		m_lighting_pso.SetMaterial(material);
		if (m_shadow_map_texture) {
			m_lighting_pso.AddShadowTexture(m_shadow_map_texture);
		}

		m_lighting_pso.Apply(m_command_list);
		
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

