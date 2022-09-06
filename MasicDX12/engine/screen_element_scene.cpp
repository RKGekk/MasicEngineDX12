#include "screen_element_scene.h"

ScreenElementScene::ScreenElementScene() : Scene() {}

void ScreenElementScene::VOnUpdate(const GameTimerDelta& delta) {
	OnUpdate();
}

HRESULT ScreenElementScene::VOnRestore() {
	OnRestore();
	return S_OK;
}

HRESULT ScreenElementScene::VOnRender(const GameTimerDelta& delta) {
	return S_OK;
}

HRESULT ScreenElementScene::VOnLostDevice() {
	OnLostDevice();
	return S_OK;
}

int ScreenElementScene::VGetZOrder() const {
	return 0;
}

void ScreenElementScene::VSetZOrder(int const zOrder) {}

LRESULT ScreenElementScene::VOnMsgProc(HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam) {
	return 0;
}

bool ScreenElementScene::VIsVisible() const {
	return m_is_visible;
}

void ScreenElementScene::VSetVisible(bool visible) {
	m_is_visible = visible;
}

bool ScreenElementScene::VAddChild(std::shared_ptr<SceneNode> kid) {
	return Scene::AddChild(kid);
}