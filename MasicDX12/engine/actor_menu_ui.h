#pragma once

#include <string>
#include <memory>

#include <DirectXMath.h>

#include "base_ui.h"
#include "../processes/process_manager.h"
#include "../actors/actor.h"
#include "../graphics/directx12_wrappers/shader.h"

#include "../graphics/gui.h"
#include "../graphics/imgui/imgui.h"
#include "../graphics/imgui/imgui_impl_win32.h"
#include "../graphics/imgui/imgui_impl_dx12.h"
#include "../nodes/light_type.h"

class Texture;
class ShaderResourceView;
class RootSignature;
class PipelineStateObject;

class ActorMenuUI : public BaseUI {
public:
	ActorMenuUI(std::shared_ptr<ProcessManager> pm);
	virtual ~ActorMenuUI();

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList> command_list) override;
	virtual void VOnUpdate(const GameTimerDelta& delta) override;

	virtual int VGetZOrder() const override;
	virtual void VSetZOrder(int const zOrder) override;

	virtual LRESULT CALLBACK VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) override;

protected:
	void Set(std::shared_ptr<ProcessManager> pm);

private:
	bool m_show_menu = false;
	std::weak_ptr<ProcessManager> m_pm;

	int m_actor_id;

	bool m_transform_exists;
	DirectX::XMFLOAT4X4 m_transform;
	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT3 m_yaw_pith_roll;
	std::string m_actor_name;

	bool m_particle_exists;
	float m_damping;
	float m_radius;
	float m_mass;

	bool m_camera_exists;
	float m_fov;
	float m_near;
	float m_far;

	bool m_light_exists;
	LightType m_light_type;
	DirectX::XMFLOAT3 m_strength;
	float m_attenuation[3]; // Constant, LinearAttenuation, Quadratic
	float m_range;
	float m_spot;
	DirectX::XMFLOAT3 m_ambient;

	bool m_mesh_exists;
	std::string m_mesh_name;

	DirectX::XMFLOAT4 m_bg_color;
	DirectX::XMFLOAT4 m_fog_color;
	float m_fog_start;
	float m_fog_range;
};