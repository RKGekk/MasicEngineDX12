#include "actor_menu_ui.h"

#include "engine.h"
#include "../actors/actor_component.h"
#include "../actors/transform_component.h"
#include "../actors/particle_component.h"

#include "../graphics/imgui/imgui.h"
#include "../graphics/imgui/imgui_impl_win32.h"
#include "../graphics/imgui/imgui_impl_dx12.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ActorMenuUI::ActorMenuUI(std::shared_ptr<ProcessManager> pm) {
	m_show_menu = true;
	m_actor_id = 0;

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
