#pragma once

#include <memory>

#include "i_screen_element.h"
#include "../nodes/scene.h"

class ScreenElementScene : public IScreenElement, public Scene {
public:
	ScreenElementScene(IRenderer* renderer);
	virtual ~ScreenElementScene() {}

	virtual void VOnUpdate(float deltaMS) override;
	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnRender(double fTime, float fElapsedTime) override;
	virtual HRESULT VOnLostDevice() override;
	virtual int VGetZOrder() const override;
	virtual void VSetZOrder(int const zOrder) override;

	virtual LRESULT CALLBACK VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) override;

	virtual bool VIsVisible() const override;
	virtual void VSetVisible(bool visible) override;

	virtual bool VAddChild(ActorId id, ComponentId cid, std::shared_ptr<ISceneNode> kid);

private:
	bool m_is_visible = true;
};