#pragma once

#include <memory>
#include <algorithm>

#include <DirectXMath.h>

#include "i_pointer_handler.h"
#include "i_keyboard_handler.h"
#include "../nodes/scene_node.h"

class Actor;

class MovementController : public IPointerHandler, public IKeyboardHandler {
public:
	MovementController(std::shared_ptr<Actor> object, float initial_yaw, float initial_pitch, bool rotate_when_LButton_down, bool rotate_when_RButton_down);
	void SetObject(std::shared_ptr<Actor> new_object);

	void OnUpdate(const GameTimerDelta& delta);

	bool VOnPointerMove(int x, int y, const int radius) override;
	bool VOnPointerButtonDown(int x, int y, const int radius, const std::string& button_name) override;
	bool VOnPointerButtonUp(int x, int y, const int radius, const std::string& button_name) override;

	bool VOnKeyDown(const BYTE c);
	bool VOnKeyUp(const BYTE c);

	const DirectX::XMFLOAT4X4& GetToWorld4x4();
	DirectX::XMMATRIX GetToWorld();

	const DirectX::XMFLOAT4X4& GetFromWorld4x4();
	DirectX::XMMATRIX GetFromWorld();

protected:
	DirectX::XMFLOAT4X4 m_mat_from_world;
	DirectX::XMFLOAT4X4 m_mat_to_world;
	DirectX::XMFLOAT4X4 m_mat_position;

	int m_last_mouse_pos_x;
	int m_last_mouse_pos_y;
	bool m_bKey[256];

	float m_fTarget_yaw;
	float m_fTarget_pitch;
	float m_fYaw;
	float m_fPitch;
	float m_fPitch_on_down;
	float m_fYaw_on_down;
	float m_max_speed;
	float m_current_speed;

	bool m_mouse_LButton_down;
	bool m_bRotate_when_LButton_down;

	bool m_mouse_RButton_down;
	bool m_bRotate_when_RButton_down;

	std::weak_ptr<Actor> m_object;
};