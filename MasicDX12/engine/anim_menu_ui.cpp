#include "anim_menu_ui.h"

#include "engine.h"
#include "actor_animation_player.h"
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

AnimMenuUI::AnimMenuUI(std::shared_ptr<ProcessManager> pm) {
	m_show_menu = true;
	Set(pm);
}

AnimMenuUI::~AnimMenuUI() {}

HRESULT AnimMenuUI::VOnRestore() {
	return S_OK;
}

DirectX::XMFLOAT3 QuaternionEulers(const DirectX::XMFLOAT4& q) {
	float sqw = q.w * q.w;
	float sqx = q.x * q.x;
	float sqy = q.y * q.y;
	float sqz = q.z * q.z;

	return DirectX::XMFLOAT3(
		atan2f(2.0f * (q.y * q.z + q.x * q.w), (-sqx - sqy + sqz + sqw)),
		-asinf(2.0f * (q.x * q.z - q.y * q.w)),
		atan2f(2.0f * (q.x * q.y + q.z * q.w), (sqx - sqy - sqz + sqw))
	);
}

HRESULT AnimMenuUI::VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList> command_list) {
	using namespace std;
	using namespace std::literals;
	if (!m_show_menu) { return S_OK; }

	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<BaseEngineLogic> logic = engine->GetGameLogic();
	std::shared_ptr<HumanView> human_view = logic->GetHumanView();
	std::shared_ptr<Scene> scene = human_view->VGetScene();
	std::shared_ptr<BasicCameraNode> camera = human_view->VGetCamera();
	std::shared_ptr<ActorAnimationPlayer> anim_player = logic->GetAnimationPlayer();

	if (ImGui::Begin("Player Menu")) {
		float total_animation_time = anim_player->GetTotalAnimationTime();
		float current_time = anim_player->GetCurrentAnimationTime();

		if (ImGui::SliderFloat("Time", ((float*)&current_time), 0.0f, total_animation_time, "%.4f")) {
			anim_player->SetDuration(current_time);
		}

		if (ImGui::Button("Play")) {
			anim_player->Play();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop")) {
			anim_player->Pause();
		}

		ActorAnimationPlayer::AnimMap& anim_map = anim_player->GetAnimMap();
		for (auto& [actor_ptr, anim] : anim_map) {
			unsigned int act_id = actor_ptr->GetId();
			std::string act_id_str = std::to_string(act_id);
			std::string actor_name = actor_ptr->GetName();
			std::string chanel_name = "Actor "s + act_id_str + " "s + actor_name + " channel";
			if (ImGui::CollapsingHeader(chanel_name.c_str())) {
				int ct = 0u;
				if (ImGui::TreeNode("Translation channels")) {
					for (ActorAnimationPlayer::KeyframeTranslation& tkf : anim.TranslationKeyframes) {
						std::string tkf_name = "Trc "s + std::to_string(ct);
						if (ImGui::SliderFloat("T Time Point", ((float*)&tkf.TimePos), 0.0f, total_animation_time, "%.4f")) {}
						if (ImGui::SliderFloat3(tkf_name.c_str(), ((float*)&tkf.Translation), -2.0f, 2.0f)) {}
						++ct;
					}
					ImGui::TreePop();
				}
				ct = 0u;
				if (ImGui::TreeNode("Rotation channels")) {
					for (ActorAnimationPlayer::KeyframeRotation& rkf : anim.RotationKeyframes) {
						std::string rkf_name = "YPRc "s + std::to_string(ct);
						if (ImGui::SliderFloat("R Time Point", ((float*)&rkf.TimePos), 0.0f, total_animation_time, "%.4f")) {}
						DirectX::XMFLOAT3 ypr = QuaternionEulers(rkf.RotationQuat);
						ypr.x = DirectX::XMConvertToDegrees(ypr.x);
						ypr.y = DirectX::XMConvertToDegrees(ypr.y);
						ypr.z = DirectX::XMConvertToDegrees(ypr.z);
						if (ImGui::SliderFloat3(rkf_name.c_str(), ((float*)&ypr), -180.0f, 180.0f)) {
							ypr.x = DirectX::XMConvertToRadians(ypr.x);
							ypr.y = DirectX::XMConvertToRadians(ypr.y);
							ypr.z = DirectX::XMConvertToRadians(ypr.z);
							DirectX::XMFLOAT4 q;
							//DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionRotationRollPitchYaw(ypr.z, ypr.y, ypr.x));
							//DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionRotationRollPitchYaw(ypr.y, ypr.x, ypr.z));
							DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionRotationRollPitchYaw(ypr.x, ypr.y, ypr.z));
							rkf.RotationQuat = q;
						}
						++ct;
					}
					ImGui::TreePop();
				}
				ct = 0u;
				if (ImGui::TreeNode("Scale channels")) {
					for (ActorAnimationPlayer::KeyframeScale& skf : anim.ScaleKeyframes) {
						std::string skf_name = "Scc "s + std::to_string(ct);
						if (ImGui::SliderFloat("S Time Point", ((float*)&skf.TimePos), 0.0f, total_animation_time, "%.4f")) {}
						if (ImGui::SliderFloat3(skf_name.c_str(), ((float*)&skf.Scale), 0.0001f, 2.0f)) {}
						++ct;
					}
					ImGui::TreePop();
				}
			}
		}
	}
	ImGui::End();

	return S_OK;
}

void AnimMenuUI::VOnUpdate(const GameTimerDelta& delta) {}

int AnimMenuUI::VGetZOrder() const {
	return 3;
}

void AnimMenuUI::VSetZOrder(int const zOrder) {}

LRESULT AnimMenuUI::VOnMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) { return true; }
	return 0;
}

void AnimMenuUI::Set(std::shared_ptr<ProcessManager> pm) {
	m_pm = pm;
}