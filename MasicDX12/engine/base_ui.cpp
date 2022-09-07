#include "base_ui.h"

BaseUI::BaseUI() {
	m_is_visible = true;
	m_pos_x = 0;
	m_pos_y = 0;
	m_width = 100;
	m_height = 100;
}

HRESULT BaseUI::VOnLostDevice() {
	return S_OK;
}

void BaseUI::VOnUpdate(const GameTimerDelta& delta) {}

bool BaseUI::VIsVisible() const {
	return m_is_visible;
}

void BaseUI::VSetVisible(bool visible) {
	m_is_visible = visible;
}
