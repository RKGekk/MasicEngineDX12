#pragma once

#include <memory>

#include "i_screen_element.h"
#include "../nodes/scene.h"

class ScreenElementScene : public IScreenElement, public Scene {
public:
	ScreenElementScene();
	virtual ~ScreenElementScene() {}

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnLostDevice() override;

	virtual void VOnUpdate(const GameTimerDelta& delta) override;
	virtual HRESULT VOnRender(const GameTimerDelta& delta) override;
	
	virtual int VGetZOrder() const override;
	virtual void VSetZOrder(int const zOrder) override;

	virtual LRESULT CALLBACK VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) override;

	virtual bool VIsVisible() const override;
	virtual void VSetVisible(bool visible) override;

	virtual bool VAddChild(std::shared_ptr<SceneNode> kid);

private:
	bool m_is_visible = true;
};