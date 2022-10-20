#pragma once

#include <memory>

#include <d3d12.h>

#include "i_screen_element.h"
#include "../nodes/scene.h"
#include "../nodes/scene_node.h"
#include "../graphics/directx12_wrappers/render_target.h"
#include "../graphics/effect_pso.h"
#include "../graphics/effect_instanced_pso.h"
#include "../graphics/effect_shadow_pso.h"
#include "../graphics/effect_shadow_instanced_pso.h"
#include "../events/i_event_data.h"

class ScreenElementScene : public IScreenElement, public Scene {
public:
	ScreenElementScene();
	virtual ~ScreenElementScene();

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnLostDevice() override;

	virtual void VOnUpdate(const GameTimerDelta& delta) override;
	virtual HRESULT VOnRender(const GameTimerDelta& delta, std::shared_ptr<CommandList> command_list) override;
	
	virtual int VGetZOrder() const override;
	virtual void VSetZOrder(int const zOrder) override;

	virtual LRESULT CALLBACK VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) override;

	virtual bool VIsVisible() const override;
	virtual void VSetVisible(bool visible) override;

	virtual bool VAddChild(std::shared_ptr<SceneNode> kid);

	void ModifiedSceneNodeComponentDelegate(IEventDataPtr pEventData);

private:
	bool m_is_visible = true;
	RenderTarget m_render_target;
	DXGI_FORMAT m_back_buffer_format;
	DXGI_FORMAT m_depth_buffer_format;
	uint32_t m_width;
	uint32_t m_height;
	std::shared_ptr<EffectPSO> m_lighting_pso;
	std::shared_ptr<EffectInstancedPSO> m_lighting_instanced_pso;
	std::shared_ptr<EffectShadowPSO> m_shadow_pso;
	std::shared_ptr<EffectShadowInstancedPSO> m_shadow_instanced_pso;
	std::shared_ptr<Texture> m_shadow_map_texture;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissor_rect;



private:
	void RegisterAllDelegates();
	void RemoveAllDelegates();
};