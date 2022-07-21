#include "screen_element_scene.h"

ScreenElementScene::ScreenElementScene(IRenderer* renderer) : Scene(renderer) {}

void ScreenElementScene::VOnUpdate(float deltaMS) {
	OnUpdate(deltaMS);
}

HRESULT ScreenElementScene::VOnRestore() {
	OnRestore();
	return S_OK;
}

HRESULT ScreenElementScene::VOnRender(double fTime, float fElapsedTime) {
	OnRender();
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

bool ScreenElementScene::VAddChild(ActorId id, ComponentId cid, std::shared_ptr<ISceneNode> kid) {
	return Scene::AddChild(id, cid, kid);
}