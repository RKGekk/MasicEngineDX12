#pragma once

#include <string>
#include <memory>

#include <DirectXMath.h>

#include "base_ui.h"
#include "../processes/process_manager.h"
#include "../actors/actor.h"

class ActorMenuUI : public BaseUI {
public:
	ActorMenuUI(std::shared_ptr<ProcessManager> pm);
	virtual ~ActorMenuUI();

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnRender(const GameTimerDelta& delta) override;
	virtual void VOnUpdate(const GameTimerDelta& delta) override;

	virtual int VGetZOrder() const override;
	virtual void VSetZOrder(int const zOrder) override;

	virtual LRESULT CALLBACK VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) override;

protected:
	void Set(std::shared_ptr<ProcessManager>);

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
};