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
			DirectX::XMFLOAT4X4 to_parent = props.ToParent4x4();
			DirectX::XMFLOAT4X4 from_parent = props.FromParent4x4();

			DirectX::XMMATRIX to_parent_xm = DirectX::XMLoadFloat4x4(&to_parent);
			DirectX::XMVECTOR to_parent_scale_xm;
			DirectX::XMVECTOR to_parent_rotation_xm;
			DirectX::XMVECTOR to_parent_translation_xm;
			DirectX::XMMatrixDecompose(&to_parent_scale_xm, &to_parent_rotation_xm, &to_parent_translation_xm, to_parent_xm);
			DirectX::XMFLOAT3 to_parent_scale;
			DirectX::XMStoreFloat3(&to_parent_scale, to_parent_scale_xm);
			DirectX::XMFLOAT4 to_parent_rotation;
			DirectX::XMStoreFloat4(&to_parent_rotation, to_parent_rotation_xm);
			DirectX::XMFLOAT3 to_parent_translation;
			DirectX::XMStoreFloat3(&to_parent_translation, to_parent_translation_xm);

			DirectX::XMFLOAT4X4 to_root = props.ToRoot4x4();
			DirectX::XMFLOAT4X4 from_root = props.FromRoot4x4();

			DirectX::XMMATRIX to_root_xm = DirectX::XMLoadFloat4x4(&to_root);
			DirectX::XMVECTOR to_root_scale_xm;
			DirectX::XMVECTOR to_root_rotation_xm;
			DirectX::XMVECTOR to_root_translation_xm;
			DirectX::XMMatrixDecompose(&to_root_scale_xm, &to_root_rotation_xm, &to_root_translation_xm, to_root_xm);
			DirectX::XMFLOAT3 to_root_scale;
			DirectX::XMStoreFloat3(&to_root_scale, to_root_scale_xm);
			DirectX::XMFLOAT4 to_root_rotation;
			DirectX::XMStoreFloat4(&to_root_rotation, to_root_rotation_xm);
			DirectX::XMFLOAT3 to_root_translation;
			DirectX::XMStoreFloat3(&to_root_translation, to_root_translation_xm);

			uint32_t generation = props.GetGeneration();
			uint32_t group_id = props.GetGroupID();

			DirectX::BoundingBox AABB = props.AABB();
			DirectX::BoundingSphere sphere = props.Sphere();
			DirectX::BoundingBox AABB_cumulative = props.CumulativeAABB();
			DirectX::BoundingSphere sphere_cumulative = props.CumulativeSphere();
			DirectX::BoundingBox AABB_merged = props.MergedAABB();
			DirectX::BoundingSphere sphere_merged = props.MergedSphere();
			if (ImGui::TreeNode("Matrix")) {
				if (ImGui::TreeNode("ToParent")) {
					if (ImGui::InputFloat4("TR1", ((float*)&to_parent) + 0, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("TR2", ((float*)&to_parent) + 4, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("TR3", ((float*)&to_parent) + 8, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("TR4", ((float*)&to_parent) + 12, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}

					if (ImGui::InputFloat3("TSc", ((float*)&to_parent_scale), "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("TRq", ((float*)&to_parent_rotation), "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat3("TTr", ((float*)&to_parent_translation), "%.4f", ImGuiInputTextFlags_ReadOnly)) {}

					ImGui::TreePop();
				}
				if (ImGui::TreeNode("FromParent")) {
					if (ImGui::InputFloat4("FR1", ((float*)&from_parent) + 0, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("FR2", ((float*)&from_parent) + 4, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("FR3", ((float*)&from_parent) + 8, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("FR4", ((float*)&from_parent) + 12, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("ToRoot")) {
					if (ImGui::InputFloat4("RR1", ((float*)&to_root) + 0, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("RR2", ((float*)&to_root) + 4, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("RR3", ((float*)&to_root) + 8, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("RR4", ((float*)&to_root) + 12, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}

					if (ImGui::InputFloat3("RSc", ((float*)&to_root_scale), "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("RRq", ((float*)&to_root_rotation), "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat3("RTr", ((float*)&to_root_translation), "%.4f", ImGuiInputTextFlags_ReadOnly)) {}

					ImGui::TreePop();
				}
				if (ImGui::TreeNode("FromRoot")) {
					if (ImGui::InputFloat4("FRR1", ((float*)&from_root) + 0, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("FRR2", ((float*)&from_root) + 4, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("FRR3", ((float*)&from_root) + 8, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputFloat4("FRR4", ((float*)&from_root) + 12, "%.4f", ImGuiInputTextFlags_ReadOnly)) {}
					ImGui::TreePop();
				}
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

			if (ImGui::ColorEdit3("Strength", ((float*)&(strength)), ImGuiColorEditFlags_Float)) {}
			if (ImGui::InputFloat3("Attenuation", ((float*)&(attenuation)), "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::InputFloat("Range", ((float*)&(range)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::InputFloat("Spot", ((float*)&(spot)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::ColorEdit3("Ambient", ((float*)&(ambient)), ImGuiColorEditFlags_Float)) {}

			ImGui::TreePop();
		}

		if (pShadowCamera && ImGui::TreeNode("ShadowProps")) {
			const ShadowCameraNode::ShadowCameraProps& shadow_props = pShadowCamera->GetShadowProps();
			int shadow_map_width = shadow_props.ShadowMapWidth;
			int shadow_map_height = shadow_props.ShadowMapHeight;
			int depth_bias = shadow_props.DepthBias;
			float depth_bias_clamp = shadow_props.DepthBiasClamp;
			float slope_scaled_depth_bias = shadow_props.SlopeScaledDepthBias;

			
			ImGui::LabelText("Bias fn", "DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope");
			if (ImGui::InputInt("Shadowmap width", ((int*)&(shadow_map_width)), 1, 2, ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::InputInt("Shadowmap height", ((int*)&(shadow_map_height)), 1, 2, ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::InputInt("Depth bias", ((int*)&(depth_bias)), 1, 2, ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::InputFloat("Depth bias clamp", ((float*)&(depth_bias_clamp)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
			if (ImGui::InputFloat("Slope scaled depth bias", ((float*)&(slope_scaled_depth_bias)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}

			ImGui::TreePop();
		}

		if (pMesh && ImGui::TreeNode("MeshProps")) {
			std::string is_instanced_str = pMesh->GetIsInstanced() ? "Instanced" : "Sequenced";
			ImGui::LabelText("IsInstanced", is_instanced_str.c_str());
			const auto& mesh_list = pMesh->GetMeshes();
			int ct = 0;
			for (const auto& mesh : mesh_list) {
				std::string mesh_name = mesh->GetName() + std::to_string(ct++);
				if (ImGui::TreeNode(mesh_name.c_str())) {
					int slots_count = mesh->GetVertexBuffers().size();
					int vertex_count = mesh->GetVertexCountSum();
					int index_count = mesh->GetIndexCount();

					if (ImGui::InputInt("Slots", ((int*)&slots_count), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputInt("VertexCount", ((int*)&vertex_count), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}
					if (ImGui::InputInt("IndexCount", ((int*)&index_count), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}

					std::shared_ptr<Material> material = mesh->GetMaterial();
					if (material && ImGui::TreeNode("Material")) {
						std::string material_name = material->GetName();
						ImGui::LabelText("MaterialName", material_name.c_str());

						const MaterialProperties& mat_props = material->GetMaterialProperties();
						DirectX::XMFLOAT4 diffuse = mat_props.Diffuse;
						DirectX::XMFLOAT4 specular = mat_props.Specular;
						DirectX::XMFLOAT4 emissive = mat_props.Emissive;
						DirectX::XMFLOAT4 ambient = mat_props.Ambient;
						DirectX::XMFLOAT4 reflectance = mat_props.Reflectance;
						float opacity = mat_props.Opacity;
						float specular_power = mat_props.SpecularPower;
						float index_of_refraction = mat_props.IndexOfRefraction;
						float bump_intensity = mat_props.BumpIntensity;
						uint32_t has_texture = mat_props.HasTexture;

						if (ImGui::ColorEdit4("Diffuse", ((float*)&(diffuse)), ImGuiColorEditFlags_Float)) {}
						if (ImGui::ColorEdit4("Specular", ((float*)&(specular)), ImGuiColorEditFlags_Float)) {}
						if (ImGui::ColorEdit4("Emissive", ((float*)&(emissive)), ImGuiColorEditFlags_Float)) {}
						if (ImGui::ColorEdit4("Ambient", ((float*)&(ambient)), ImGuiColorEditFlags_Float)) {}
						if (ImGui::ColorEdit4("Reflectance", ((float*)&(reflectance)), ImGuiColorEditFlags_Float)) {}
						if (ImGui::InputFloat("Opacity", ((float*)&(opacity)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
						if (ImGui::InputFloat("SpecularPower", ((float*)&(specular_power)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
						if (ImGui::InputFloat("IndexOfRefraction", ((float*)&(index_of_refraction)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
						if (ImGui::InputFloat("BumpIntensity", ((float*)&(bump_intensity)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
						if (ImGui::InputFloat("BumpIntensity", ((float*)&(bump_intensity)), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly)) {}
						if (ImGui::InputInt("TextureFlags", ((int*)&has_texture), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}

						for (auto const& [texture_type, pTexture] : material->GetTextureMap()) {
							std::string texture_type_str = "Unknown";
							switch (texture_type) {
								case Material::TextureType::Ambient: texture_type_str = "Ambient";  break;
								case Material::TextureType::Emissive: texture_type_str = "Emissive"; break;
								case Material::TextureType::Diffuse: texture_type_str = "Diffuse"; break;
								case Material::TextureType::Specular: texture_type_str = "Specular"; break;
								case Material::TextureType::SpecularPower: texture_type_str = "SpecularPower"; break;
								case Material::TextureType::Normal: texture_type_str = "Normal"; break;
								case Material::TextureType::Bump: texture_type_str = "Bump"; break;
								case Material::TextureType::Opacity: texture_type_str = "Opacity"; break;
								case Material::TextureType::Displacement: texture_type_str = "Displacement"; break;
								case Material::TextureType::Metalness: texture_type_str = "Metalness"; break;
								case Material::TextureType::Shadow: texture_type_str = "Shadow"; break;
							}
							if (ImGui::TreeNode(texture_type_str.c_str())) {
								D3D12_RESOURCE_DESC desc = pTexture->GetD3D12ResourceDesc();
								size_t bpp = pTexture->BitsPerPixel();
								std::string dimension_str = "unknown";
								switch (desc.Dimension) {
									case D3D12_RESOURCE_DIMENSION_BUFFER: dimension_str = "buffer"; break;
									case D3D12_RESOURCE_DIMENSION_TEXTURE1D: dimension_str = "texture1D"; break;
									case D3D12_RESOURCE_DIMENSION_TEXTURE2D: dimension_str = "texture2D"; break;
									case D3D12_RESOURCE_DIMENSION_TEXTURE3D: dimension_str = "texture3D"; break;
								}
								UINT64 alignment = desc.Alignment;
								UINT64 width = desc.Width;
								UINT height = desc.Height;
								UINT16 depth_or_array_size = desc.DepthOrArraySize;
								UINT16 mip_levels = desc.MipLevels;
								DXGI_FORMAT format = desc.Format;
								std::string format_str = "unknown";
								switch (desc.Format) {
									case DXGI_FORMAT_R32G32B32A32_TYPELESS: format_str = "R32G32B32A32_TYPELESS"; break;
									case DXGI_FORMAT_R32G32B32A32_FLOAT: format_str = "R32G32B32A32_FLOAT"; break;
									case DXGI_FORMAT_R32G32B32A32_UINT: format_str = "R32G32B32A32_UINT"; break;
									case DXGI_FORMAT_R32G32B32A32_SINT: format_str = "R32G32B32A32_SINT"; break;
									case DXGI_FORMAT_R32G32B32_TYPELESS: format_str = "R32G32B32_TYPELESS"; break;
									case DXGI_FORMAT_R32G32B32_FLOAT: format_str = "R32G32B32_FLOAT"; break;
									case DXGI_FORMAT_R32G32B32_UINT: format_str = "R32G32B32_UINT"; break;
									case DXGI_FORMAT_R32G32B32_SINT: format_str = "R32G32B32_SINT"; break;
									case DXGI_FORMAT_R16G16B16A16_TYPELESS: format_str = "R16G16B16A16_TYPELESS"; break;
									case DXGI_FORMAT_R16G16B16A16_FLOAT: format_str = "R16G16B16A16_FLOAT"; break;
									case DXGI_FORMAT_R16G16B16A16_UNORM: format_str = "R16G16B16A16_UNORM"; break;
									case DXGI_FORMAT_R16G16B16A16_UINT: format_str = "R16G16B16A16_UINT"; break;
									case DXGI_FORMAT_R16G16B16A16_SNORM: format_str = "R16G16B16A16_SNORM"; break;
									case DXGI_FORMAT_R16G16B16A16_SINT: format_str = "R16G16B16A16_SINT"; break;
									case DXGI_FORMAT_R32G32_TYPELESS: format_str = "R32G32_TYPELESS"; break;
									case DXGI_FORMAT_R32G32_FLOAT: format_str = "R32G32_FLOAT"; break;
									case DXGI_FORMAT_R32G32_UINT: format_str = "R32G32_UINT"; break;
									case DXGI_FORMAT_R32G32_SINT: format_str = "R32G32_SINT"; break;
									case DXGI_FORMAT_R32G8X24_TYPELESS: format_str = "R32G8X24_TYPELESS"; break;
									case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: format_str = "D32_FLOAT_S8X24_UINT"; break;
									case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: format_str = "R32_FLOAT_X8X24_TYPELESS"; break;
									case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: format_str = "X32_TYPELESS_G8X24_UINT"; break;
									case DXGI_FORMAT_R10G10B10A2_TYPELESS: format_str = "R10G10B10A2_TYPELESS"; break;
									case DXGI_FORMAT_R10G10B10A2_UNORM: format_str = "R10G10B10A2_UNORM"; break;
									case DXGI_FORMAT_R10G10B10A2_UINT: format_str = "R10G10B10A2_UINT"; break;
									case DXGI_FORMAT_R11G11B10_FLOAT: format_str = "R11G11B10_FLOAT"; break;
									case DXGI_FORMAT_R8G8B8A8_TYPELESS: format_str = "R8G8B8A8_TYPELESS"; break;
									case DXGI_FORMAT_R8G8B8A8_UNORM: format_str = "R8G8B8A8_UNORM"; break;
									case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: format_str = "R8G8B8A8_UNORM_SRGB"; break;
									case DXGI_FORMAT_R8G8B8A8_UINT: format_str = "R8G8B8A8_UINT"; break;
									case DXGI_FORMAT_R8G8B8A8_SNORM: format_str = "R8G8B8A8_SNORM"; break;
									case DXGI_FORMAT_R8G8B8A8_SINT: format_str = "R8G8B8A8_SINT"; break;
									case DXGI_FORMAT_R16G16_TYPELESS: format_str = "R16G16_TYPELESS"; break;
									case DXGI_FORMAT_R16G16_FLOAT: format_str = "R16G16_FLOAT"; break;
									case DXGI_FORMAT_R16G16_UNORM: format_str = "R16G16_UNORM"; break;
									case DXGI_FORMAT_R16G16_UINT: format_str = "R16G16_UINT"; break;
									case DXGI_FORMAT_R16G16_SNORM: format_str = "R16G16_SNORM"; break;
									case DXGI_FORMAT_R16G16_SINT: format_str = "R16G16_SINT"; break;
									case DXGI_FORMAT_R32_TYPELESS: format_str = "R32_TYPELESS"; break;
									case DXGI_FORMAT_D32_FLOAT: format_str = "D32_FLOAT"; break;
									case DXGI_FORMAT_R32_FLOAT: format_str = "R32_FLOAT"; break;
									case DXGI_FORMAT_R32_UINT: format_str = "R32_UINT"; break;
									case DXGI_FORMAT_R32_SINT: format_str = "R32_SINT"; break;
									case DXGI_FORMAT_R24G8_TYPELESS: format_str = "R24G8_TYPELESS"; break;
									case DXGI_FORMAT_D24_UNORM_S8_UINT: format_str = "D24_UNORM_S8_UINT"; break;
									case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: format_str = "R24_UNORM_X8_TYPELESS"; break;
									case DXGI_FORMAT_X24_TYPELESS_G8_UINT: format_str = "X24_TYPELESS_G8_UINT"; break;
									case DXGI_FORMAT_R8G8_TYPELESS: format_str = "R8G8_TYPELESS"; break;
									case DXGI_FORMAT_R8G8_UNORM: format_str = "R8G8_UNORM"; break;
									case DXGI_FORMAT_R8G8_UINT: format_str = "R8G8_UINT"; break;
									case DXGI_FORMAT_R8G8_SNORM: format_str = "R8G8_SNORM"; break;
									case DXGI_FORMAT_R8G8_SINT: format_str = "R8G8_SINT"; break;
									case DXGI_FORMAT_R16_TYPELESS: format_str = "R16_TYPELESS"; break;
									case DXGI_FORMAT_R16_FLOAT: format_str = "R16_FLOAT"; break;
									case DXGI_FORMAT_D16_UNORM: format_str = "D16_UNORM"; break;
									case DXGI_FORMAT_R16_UNORM: format_str = "R16_UNORM"; break;
									case DXGI_FORMAT_R16_UINT: format_str = "R16_UINT"; break;
									case DXGI_FORMAT_R16_SNORM: format_str = "R16_SNORM"; break;
									case DXGI_FORMAT_R16_SINT: format_str = "R16_SINT"; break;
									case DXGI_FORMAT_R8_TYPELESS: format_str = "R8_TYPELESS"; break;
									case DXGI_FORMAT_R8_UNORM: format_str = "R8_UNORM"; break;
									case DXGI_FORMAT_R8_UINT: format_str = "R8_UINT"; break;
									case DXGI_FORMAT_R8_SNORM: format_str = "R8_SNORM"; break;
									case DXGI_FORMAT_R8_SINT: format_str = "R8_SINT"; break;
									case DXGI_FORMAT_A8_UNORM: format_str = "A8_UNORM"; break;
									case DXGI_FORMAT_R1_UNORM: format_str = "R1_UNORM"; break;
									case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: format_str = "R9G9B9E5_SHAREDEXP"; break;
									case DXGI_FORMAT_R8G8_B8G8_UNORM: format_str = "R8G8_B8G8_UNORM"; break;
									case DXGI_FORMAT_G8R8_G8B8_UNORM: format_str = "G8R8_G8B8_UNORM"; break;
									case DXGI_FORMAT_BC1_TYPELESS: format_str = "BC1_TYPELESS"; break;
									case DXGI_FORMAT_BC1_UNORM: format_str = "BC1_UNORM"; break;
									case DXGI_FORMAT_BC1_UNORM_SRGB: format_str = "BC1_UNORM_SRGB"; break;
									case DXGI_FORMAT_BC2_TYPELESS: format_str = "BC2_TYPELESS"; break;
									case DXGI_FORMAT_BC2_UNORM: format_str = "BC2_UNORM"; break;
									case DXGI_FORMAT_BC2_UNORM_SRGB: format_str = "BC2_UNORM_SRGB"; break;
									case DXGI_FORMAT_BC3_TYPELESS: format_str = "BC3_TYPELESS"; break;
									case DXGI_FORMAT_BC3_UNORM: format_str = "BC3_UNORM"; break;
									case DXGI_FORMAT_BC3_UNORM_SRGB: format_str = "BC3_UNORM_SRGB"; break;
									case DXGI_FORMAT_BC4_TYPELESS: format_str = "BC4_TYPELESS"; break;
									case DXGI_FORMAT_BC4_UNORM: format_str = "BC4_UNORM"; break;
									case DXGI_FORMAT_BC4_SNORM: format_str = "BC4_SNORM"; break;
									case DXGI_FORMAT_BC5_TYPELESS: format_str = "BC5_TYPELESS"; break;
									case DXGI_FORMAT_BC5_UNORM: format_str = "BC5_UNORM"; break;
									case DXGI_FORMAT_BC5_SNORM: format_str = "BC5_SNORM"; break;
									case DXGI_FORMAT_B5G6R5_UNORM: format_str = "B5G6R5_UNORM"; break;
									case DXGI_FORMAT_B5G5R5A1_UNORM: format_str = "B5G5R5A1_UNORM"; break;
									case DXGI_FORMAT_B8G8R8A8_UNORM: format_str = "B8G8R8A8_UNORM"; break;
									case DXGI_FORMAT_B8G8R8X8_UNORM: format_str = "B8G8R8X8_UNORM"; break;
									case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: format_str = "R10G10B10_XR_BIAS_A2_UNORM"; break;
									case DXGI_FORMAT_B8G8R8A8_TYPELESS: format_str = "B8G8R8A8_TYPELESS"; break;
									case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: format_str = "B8G8R8A8_UNORM_SRGB"; break;
									case DXGI_FORMAT_B8G8R8X8_TYPELESS: format_str = "B8G8R8X8_TYPELESS"; break;
									case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: format_str = "B8G8R8X8_UNORM_SRGB"; break;
									case DXGI_FORMAT_BC6H_TYPELESS: format_str = "BC6H_TYPELESS"; break;
									case DXGI_FORMAT_BC6H_UF16: format_str = "BC6H_UF16"; break;
									case DXGI_FORMAT_BC6H_SF16: format_str = "BC6H_SF16"; break;
									case DXGI_FORMAT_BC7_TYPELESS: format_str = "BC7_TYPELESS"; break;
									case DXGI_FORMAT_BC7_UNORM: format_str = "BC7_UNORM"; break;
									case DXGI_FORMAT_BC7_UNORM_SRGB: format_str = "BC7_UNORM_SRGB"; break;
									case DXGI_FORMAT_AYUV: format_str = "AYUV"; break;
									case DXGI_FORMAT_Y410: format_str = "Y410"; break;
									case DXGI_FORMAT_Y416: format_str = "Y416"; break;
									case DXGI_FORMAT_NV12: format_str = "NV12"; break;
									case DXGI_FORMAT_P010: format_str = "P010"; break;
									case DXGI_FORMAT_P016: format_str = "P016"; break;
									case DXGI_FORMAT_420_OPAQUE: format_str = "420_OPAQUE"; break;
									case DXGI_FORMAT_YUY2: format_str = "YUY2"; break;
									case DXGI_FORMAT_Y210: format_str = "Y210"; break;
									case DXGI_FORMAT_Y216: format_str = "Y216"; break;
									case DXGI_FORMAT_NV11: format_str = "NV11"; break;
									case DXGI_FORMAT_AI44: format_str = "AI44"; break;
									case DXGI_FORMAT_IA44: format_str = "IA44"; break;
									case DXGI_FORMAT_P8: format_str = "P8"; break;
									case DXGI_FORMAT_A8P8: format_str = "A8P8"; break;
									case DXGI_FORMAT_B4G4R4A4_UNORM: format_str = "B4G4R4A4_UNORM"; break;
									case DXGI_FORMAT_P208: format_str = "P208"; break;
									case DXGI_FORMAT_V208: format_str = "V208"; break;
									case DXGI_FORMAT_V408: format_str = "V408"; break;
									case DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE: format_str = "SAMPLER_FEEDBACK_MIN_MIP_OPAQUE"; break;
									case DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE: format_str = "SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE"; break;
								}
								DXGI_SAMPLE_DESC sample_desc = desc.SampleDesc;
								std::string sample_desc_str = std::to_string(sample_desc.Count) + "/" + std::to_string(sample_desc.Quality);
								D3D12_TEXTURE_LAYOUT layout = desc.Layout;
								D3D12_RESOURCE_FLAGS flags = desc.Flags;

								if (ImGui::InputInt("BitsPerPixel", ((int*)&bpp), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}
								ImGui::LabelText("Dimension", dimension_str.c_str());
								if (ImGui::InputInt("Alignment", ((int*)&alignment), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}
								if (ImGui::InputInt("Width", ((int*)&width), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}
								if (ImGui::InputInt("Height", ((int*)&height), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}
								if (ImGui::DragScalar("DepthOrArraySize", ImGuiDataType_U16, (void*)&depth_or_array_size, 0, nullptr, nullptr, "%u")) {};
								if (ImGui::DragScalar("MipLevels", ImGuiDataType_U16, (void*)&mip_levels, 0, nullptr, nullptr, "%u")) {};
								ImGui::LabelText("Format", format_str.c_str());
								ImGui::LabelText("Sample", sample_desc_str.c_str());
								if (ImGui::InputInt("Flags", ((int*)&flags), 0, 0, ImGuiInputTextFlags_ReadOnly)) {}

								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Childs")) {
			for (auto& next_node : current_node->VGetChildren()) {
				DrawNodes(next_node);
			}
			ImGui::TreePop();
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
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) { return true; }
	return 0;
}

void NodeMenuUI::Set(std::shared_ptr<ProcessManager> pm) {
	m_pm = pm;
}