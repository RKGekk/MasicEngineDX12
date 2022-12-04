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

HRESULT AnimMenuUI::VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList> command_list) {
	using namespace std;
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
	}
	ImGui::End();

	return S_OK;
}

void AnimMenuUI::VOnUpdate(const GameTimerDelta& delta) {}

int AnimMenuUI::VGetZOrder() const {
	return 2;
}

void AnimMenuUI::VSetZOrder(int const zOrder) {}

LRESULT AnimMenuUI::VOnMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) { return true; }
	return 0;
}

void AnimMenuUI::Set(std::shared_ptr<ProcessManager> pm) {
	m_pm = pm;
}