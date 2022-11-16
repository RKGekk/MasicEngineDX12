#include "node_menu_ui.h"

#include "engine.h"
#include "../actors/actor_component.h"
#include "../actors/transform_component.h"
#include "../actors/particle_component.h"
#include "../actors/camera_component.h"
#include "../actors/light_component.h"
#include "../nodes/aminated_mesh_node.h"
#include "../nodes/camera_node.h"
#include "../nodes/light_node.h"
#include "../nodes/mesh_node.h"
#include "../nodes/qualifier_node.h"
#include "../nodes/shadow_camera_node.h"
#include "../actors/mesh_component.h"
#include "../graphics/d3d12_renderer.h"
#include "../graphics/i_renderer.h"
#include "../graphics/directx12_wrappers/command_queue.h"
#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/directx12_wrappers/swap_chain.h"
#include "../graphics/directx12_wrappers/texture.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void GetSurfaceInfo(_In_ size_t width, _In_ size_t height, _In_ DXGI_FORMAT fmt, size_t* out_num_bytes, _Out_opt_ size_t* out_row_bytes, _Out_opt_ size_t* out_num_rows);

NodeMenuUI::NodeMenuUI(std::shared_ptr<ProcessManager> pm) {
	m_show_menu = true;
	Set(pm);
}

NodeMenuUI::~NodeMenuUI() {}

HRESULT NodeMenuUI::VOnRestore() {
	return S_OK;
}

void DrawNodes(const std::shared_ptr<SceneNode>& current_node) {

	const SceneNodeProperties& props = current_node->Get();
	std::string node_name = props.Name();

	std::shared_ptr<QualifierNode> pQualifier =  nullptr;
	if (pQualifier = std::dynamic_pointer_cast<QualifierNode>(current_node)) {
		node_name += "@Qualifier";
	};

	std::shared_ptr<LightNode> pLight = nullptr;
	if (pLight = std::dynamic_pointer_cast<LightNode>(current_node)) {
		node_name += "@Light";
	};

	std::shared_ptr<CameraNode> pCamera = nullptr;
	std::shared_ptr<ShadowCameraNode> pShadowCamera = nullptr;
	if (pCamera = std::dynamic_pointer_cast<CameraNode>(current_node)) {
		if (pShadowCamera = std::dynamic_pointer_cast<ShadowCameraNode>(current_node)) {
			node_name += "@ShadowCamera";
		}
		else {
			node_name += "@Camera";
		}
	};

	std::shared_ptr<MeshNode> pMesh = nullptr;
	if (pMesh = std::dynamic_pointer_cast<MeshNode>(current_node)) {
		node_name += "@Mesh";
	};

	std::shared_ptr<AnimatedMeshNode> pAnimMesh = nullptr;
	if (pAnimMesh = std::dynamic_pointer_cast<AnimatedMeshNode>(current_node)) {
		node_name += "@AnimatedMesh";
	};

	if (ImGui::TreeNode(node_name.c_str())) {
		if (ImGui::TreeNode("Props")) {
			DirectX::XMFLOAT4X4 to_world = props.ToWorld4x4();
			DirectX::XMFLOAT4X4 from_world = props.FromWorld4x4();
			DirectX::XMFLOAT4X4 to_world_cumulative = props.CumulativeToWorld4x4();
			DirectX::XMFLOAT4X4 from_world_cumulative = props.CumulativeFromWorld4x4();

			DirectX::XMFLOAT3 scale = props.Scale3();
			DirectX::XMFLOAT3 scale_cumulative = props.CumulativeScale3();
			uint32_t generation = props.GetGeneration();
			uint32_t group_id = props.GetGroupID();

			DirectX::BoundingBox AABB = props.AABB();
			DirectX::BoundingSphere sphere = props.Sphere();
			DirectX::BoundingBox AABB_cumulative = props.CumulativeAABB();
			DirectX::BoundingSphere sphere_cumulative = props.CumulativeSphere();
			DirectX::BoundingBox AABB_merged = props.MergedAABB();
			DirectX::BoundingSphere sphere_merged = props.MergedSphere();
			if (ImGui::TreeNode("Matrix")) {
				if (ImGui::TreeNode("ToWorld")) {
					if (ImGui::InputFloat4("R1", ((float*)&to_world) + 0, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R2", ((float*)&to_world) + 4, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R3", ((float*)&to_world) + 8, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R4", ((float*)&to_world) + 12, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("FromWorld")) {
					if (ImGui::InputFloat4("R1", ((float*)&from_world) + 0, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R2", ((float*)&from_world) + 4, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R3", ((float*)&from_world) + 8, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R4", ((float*)&from_world) + 12, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("CumulativeToWorld")) {
					if (ImGui::InputFloat4("R1", ((float*)&to_world_cumulative) + 0, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R2", ((float*)&to_world_cumulative) + 4, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R3", ((float*)&to_world_cumulative) + 8, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R4", ((float*)&to_world_cumulative) + 12, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("CumulativeFromWorld")) {
					if (ImGui::InputFloat4("R1", ((float*)&from_world_cumulative) + 0, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R2", ((float*)&from_world_cumulative) + 4, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R3", ((float*)&from_world_cumulative) + 8, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("R4", ((float*)&from_world_cumulative) + 12, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Scale")) {
				if (ImGui::InputFloat("Scale", ((float*)&scale), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
				if (ImGui::InputFloat("CumulativeScale", ((float*)&scale_cumulative), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("ID")) {
				if (ImGui::InputInt("Generation", ((int*)&generation), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}
				if (ImGui::InputInt("Group", ((int*)&group_id), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Bounding")) {
				if (ImGui::TreeNode("AABB")) {
					if (ImGui::InputFloat3("Center", ((float*)&(AABB.Center)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat3("Extents", ((float*)&(AABB.Extents)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Sphere")) {
					if (ImGui::InputFloat3("Center", ((float*)&(sphere.Center)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat("Radius", ((float*)&(sphere.Radius)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("AABBCumulative")) {
					if (ImGui::InputFloat3("Center", ((float*)&(AABB_cumulative.Center)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat3("Extents", ((float*)&(AABB_cumulative.Extents)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("SphereCumulative")) {
					if (ImGui::InputFloat3("Center", ((float*)&(sphere_cumulative.Center)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat("Radius", ((float*)&(sphere_cumulative.Radius)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("AABBMerged")) {
					if (ImGui::InputFloat3("Center", ((float*)&(AABB_merged.Center)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat3("Extents", ((float*)&(AABB_merged.Extents)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("SphereMerged")) {
					if (ImGui::InputFloat3("Center", ((float*)&(sphere_merged.Center)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat("Radius", ((float*)&(sphere_merged.Radius)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (pLight && ImGui::TreeNode("LightProps")) {
			const LightProperties& light_props = pLight->VGetLight();
			LightType light_type = light_props.m_light_type;
			std::string light_type_str = "";
			switch (light_type) {
				case LightType::DIRECTIONAL: light_type_str = "Directional"; break;
				case LightType::POINT: light_type_str = "Point"; break;
				case LightType::SPOT: light_type_str = "Spot"; break;
				default: light_type_str = "Unknown"; break;
			}
			DirectX::XMFLOAT3 strength = light_props.m_strength;
			float attenuation[3];
			attenuation[0] = light_props.m_attenuation[0]; // Constant
			attenuation[1] = light_props.m_attenuation[1]; // LinearAttenuation
			attenuation[2] = light_props.m_attenuation[2]; // Quadratic
			float range = light_props.m_range;
			float spot = light_props.m_spot;
			DirectX::XMFLOAT3 ambient = light_props.m_ambient;

			ImGui::LabelText("LightType", light_type_str.c_str());
			if (ImGui::ColorEdit3("Strength", ((float*)&(strength)), ImGuiColorEditFlags_NoInputs)) {}
			if (ImGui::InputFloat3("Attenuation", ((float*)&(attenuation)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::InputFloat("Range", ((float*)&(range)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::InputFloat("Spot", ((float*)&(spot)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::ColorEdit3("Ambient", ((float*)&(ambient)), ImGuiColorEditFlags_NoInputs)) {}

			ImGui::TreePop();
		}

		if (pMesh && ImGui::TreeNode("MeshProps")) {
			std::string is_instanced_str = pMesh->GetIsInstanced() ? "Instanced" : "Sequenced";
			ImGui::LabelText("IsInstanced", is_instanced_str.c_str());
			std::vector<const char*> items;
			const auto& mesh_list = pMesh->GetMeshes();
			items.reserve(mesh_list.size());
			for (const auto& mesh : mesh_list) items.push_back(mesh->GetName().c_str());
			static int item_current = 1;
			std::string mesh_names = "MeshNames:" + std::to_string(items.size());
			ImGui::ListBox(mesh_names.c_str(), &item_current, items.data(), items.size(), 4);

			ImGui::TreePop();
		}



		for (auto& next_node : current_node->VGetChildren()) {
			DrawNodes(next_node);
		}

		ImGui::TreePop();
	}
}

HRESULT NodeMenuUI::VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList> command_list) {
	using namespace std;
	if (!m_show_menu) { return S_OK; }

	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<BaseEngineLogic> logic = engine->GetGameLogic();
	std::shared_ptr<HumanView> human_view = logic->GetHumanView();
	std::shared_ptr<Scene> scene = human_view->VGetScene();
	std::shared_ptr<BasicCameraNode> camera = human_view->VGetCamera();

	if (ImGui::Begin("Nodes Menu")) {
		DrawNodes(scene->GetRootCast());
	}
	ImGui::End();

	return S_OK;
}

void NodeMenuUI::VOnUpdate(const GameTimerDelta& delta) {}

int NodeMenuUI::VGetZOrder() const {
	return 2;
}

void NodeMenuUI::VSetZOrder(int const zOrder) {}

LRESULT NodeMenuUI::VOnMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) { return true; }
	return 0;
}

void NodeMenuUI::Set(std::shared_ptr<ProcessManager> pm) {
	m_pm = pm;
}