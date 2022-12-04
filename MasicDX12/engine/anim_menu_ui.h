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

class AnimMenuUI : public BaseUI {
public:
	AnimMenuUI(std::shared_ptr<ProcessManager> pm);
	virtual ~AnimMenuUI();

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
};