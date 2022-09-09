#define WIN32_LEAN_AND_MEAN
#include "movement_controller.h"

#include "../actors/transform_component.h"
#include "../actors/mesh_component.h"

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif


MovementController::MovementController(std::shared_ptr<Actor> object, float initial_yaw, float initial_pitch, bool rotate_when_LButton_down, bool rotate_when_RButton_down) : m_object(object) {
	std::shared_ptr<TransformComponent> tc;
	std::shared_ptr<MeshComponent> mc;
	if (object) {
		tc = object->GetComponent<TransformComponent>().lock();
		mc = object->GetComponent<MeshComponent>().lock();
	}
	if (mc) {
		std::shared_ptr<SceneNode> scene_node = mc->VGetSceneNode();
		if (scene_node) {
			m_mat_to_world = scene_node->Get().ToWorld4x4();
			m_mat_from_world = scene_node->Get().FromWorld4x4();
		}
	}
	else {
		if (tc) {
			m_mat_to_world = tc->GetTransform4x4f();
			m_mat_from_world = tc->GetInvTransform4x4f();
		}
		else {
			DirectX::XMStoreFloat4x4(&m_mat_to_world, DirectX::XMMatrixIdentity());
			DirectX::XMStoreFloat4x4(&m_mat_from_world, DirectX::XMMatrixIdentity());
		}
	}

	m_fTarget_yaw = m_fYaw = DirectX::XMConvertToDegrees(-initial_yaw);
	m_fTarget_pitch = m_fPitch = DirectX::XMConvertToDegrees(initial_pitch);

	m_max_speed = 30.0f;
	m_current_speed = 0.0f;

	DirectX::XMStoreFloat4x4(&m_mat_position, DirectX::XMMatrixIdentity());
	m_mat_position._41 = m_mat_to_world._41;
	m_mat_position._42 = m_mat_to_world._42;
	m_mat_position._43 = m_mat_to_world._43;
	m_mat_position._44 = m_mat_to_world._44;

	POINT ptCursor;
	GetCursorPos(&ptCursor);
	m_last_mouse_pos_x = ptCursor.x;
	m_last_mouse_pos_y = ptCursor.y;
	//ShowCursor(true);

	memset(m_bKey, 0x00, sizeof(m_bKey));

	m_mouse_LButton_down = false;
	m_bRotate_when_LButton_down = rotate_when_LButton_down;

	m_mouse_RButton_down = false;
	m_bRotate_when_RButton_down = rotate_when_RButton_down;
}

void MovementController::SetObject(std::shared_ptr<Actor> new_object) {
	m_object = new_object;
}

void MovementController::OnUpdate(const GameTimerDelta& delta) {
	using namespace DirectX;
	float elapsed_seconds = delta.fGetDeltaSeconds();
	if (m_object.expired()) return;
	std::shared_ptr<Actor> act = m_object.lock();
	std::shared_ptr<TransformComponent> tc;
	std::shared_ptr<MeshComponent> mc;
	if (act) {
		tc = act->GetComponent<TransformComponent>().lock();
		mc = act->GetComponent<MeshComponent>().lock();
	}

	bool bTranslating = false;
	DirectX::XMFLOAT4 atWorld(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 rightWorld(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 upWorld(0.0f, 0.0f, 0.0f, 0.0f);

	if (m_bKey['W'] || m_bKey['S']) {
		DirectX::XMFLOAT4 at(0.0f, 0.0f, 1.0f, 0.0f);
		if (m_bKey['S']) {
			at.x *= -1.0f;
			at.y *= -1.0f;
			at.z *= -1.0f;
		}
		DirectX::XMVECTOR at_xm = DirectX::XMLoadFloat4(&at);
		DirectX::XMStoreFloat4(&atWorld, DirectX::XMVector4Transform(at_xm, DirectX::XMLoadFloat4x4(&m_mat_to_world)));
		bTranslating = true;
	}

	if (m_bKey['A'] || m_bKey['D']) {
		DirectX::XMFLOAT4 right(1.0f, 0.0f, 0.0f, 0.0f);
		if (m_bKey['A']) {
			right.x *= -1.0f;
			right.y *= -1.0f;
			right.z *= -1.0f;
		}
		DirectX::XMVECTOR right_xm = DirectX::XMLoadFloat4(&right);
		DirectX::XMStoreFloat4(&rightWorld, DirectX::XMVector4Transform(right_xm, DirectX::XMLoadFloat4x4(&m_mat_to_world)));
		bTranslating = true;
	}

	if (m_bKey[' '] || m_bKey['C'] || m_bKey['X']) {
		DirectX::XMFLOAT4 up(0.0f, 1.0f, 0.0f, 0.0f);
		if (!m_bKey[' ']) {
			up.x *= -1.0f;
			up.y *= -1.0f;
			up.z *= -1.0f;
		}

		upWorld = up;
		bTranslating = true;
	}

	{
		m_fYaw += (m_fTarget_yaw - m_fYaw) * (0.35f);
		m_fTarget_pitch = std::max(-90.0f, std::min(90.0f, m_fTarget_pitch));
		m_fPitch += (m_fTarget_pitch - m_fPitch) * (0.35f);

		DirectX::XMMATRIX mat_rot = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_fPitch), DirectX::XMConvertToRadians(-m_fYaw), 0.0f);
		mat_rot.r[3] = DirectX::XMVectorSet(m_mat_position._41, m_mat_position._42, m_mat_position._43, m_mat_position._44);
		DirectX::XMStoreFloat4x4(&m_mat_to_world, mat_rot);
		DirectX::XMStoreFloat4x4(&m_mat_from_world, DirectX::XMMatrixInverse(nullptr, mat_rot));
		if (tc) {
			tc->SetTransform(m_mat_to_world);
		}
		else {
			if (mc) {
				std::shared_ptr<SceneNode> scene_node = mc->VGetSceneNode();
				if(scene_node) scene_node->SetTransform4x4(&m_mat_to_world, &m_mat_from_world);
			}
		}
	}

	if (bTranslating) {
		DirectX::XMVECTOR direction = DirectX::XMLoadFloat4(&atWorld) + DirectX::XMLoadFloat4(&rightWorld) + DirectX::XMLoadFloat4(&upWorld);
		direction = DirectX::XMVector3Normalize(direction);

		float numberOfSeconds = 5.0f;
		m_current_speed += m_max_speed * ((elapsed_seconds * elapsed_seconds) / numberOfSeconds);
		if (m_current_speed > m_max_speed) { m_current_speed = m_max_speed; }

		direction *= m_current_speed;

		DirectX::XMVECTOR pos = DirectX::XMVectorSet(m_mat_position._41, m_mat_position._42, m_mat_position._43, m_mat_position._44) + direction;
		m_mat_position._41 = DirectX::XMVectorGetX(pos);
		m_mat_position._42 = DirectX::XMVectorGetY(pos);
		m_mat_position._43 = DirectX::XMVectorGetZ(pos);

		m_mat_to_world._41 = DirectX::XMVectorGetX(pos);
		m_mat_to_world._42 = DirectX::XMVectorGetY(pos);
		m_mat_to_world._43 = DirectX::XMVectorGetZ(pos);

		DirectX::XMStoreFloat4x4(&m_mat_from_world, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&m_mat_to_world)));
		if (tc) {
			tc->SetTransform(m_mat_to_world);
		}
		else {
			if (mc) {
				std::shared_ptr<SceneNode> scene_node = mc->VGetSceneNode();
				if (scene_node) scene_node->SetTransform4x4(&m_mat_to_world, &m_mat_from_world);
			}
		}
	}
	else {
		m_current_speed = 0.0f;
	}
}

bool MovementController::VOnPointerMove(int x, int y, const int radius) {
	if (m_bRotate_when_LButton_down) {
		if ((m_last_mouse_pos_x != x || m_last_mouse_pos_y != y) && m_mouse_LButton_down) {
			m_fTarget_yaw = m_fTarget_yaw + (m_last_mouse_pos_x - x);
			m_fTarget_pitch = m_fTarget_pitch + (y - m_last_mouse_pos_y);
			m_last_mouse_pos_x = x;
			m_last_mouse_pos_y = y;
		}
	}
	else if (m_bRotate_when_RButton_down) {
		if ((m_last_mouse_pos_x != x || m_last_mouse_pos_y != y) && m_mouse_RButton_down) {
			m_fTarget_yaw = m_fTarget_yaw + (m_last_mouse_pos_x - x);
			m_fTarget_pitch = m_fTarget_pitch + (y - m_last_mouse_pos_y);
			m_last_mouse_pos_x = x;
			m_last_mouse_pos_y = y;
		}
	}
	else if (m_last_mouse_pos_x != x || m_last_mouse_pos_y != y) {
		m_fTarget_yaw = m_fTarget_yaw + (m_last_mouse_pos_x - x);
		m_fTarget_pitch = m_fTarget_pitch + (y - m_last_mouse_pos_y);
		m_last_mouse_pos_x = x;
		m_last_mouse_pos_y = y;
	}

	return true;
}

bool MovementController::VOnPointerButtonDown(int x, int y, const int radius, const std::string& button_name) {
	if (button_name == "PointerLeft") {
		m_mouse_LButton_down = true;
		m_last_mouse_pos_x = x;
		m_last_mouse_pos_y = y;
		return true;
	}
	if (button_name == "PointerRight") {
		m_mouse_RButton_down = true;
		m_last_mouse_pos_x = x;
		m_last_mouse_pos_y = y;
		return true;
	}
	return false;
}

bool MovementController::VOnPointerButtonUp(int x, int y, const int radius, const std::string& button_name) {
	if (button_name == "PointerLeft") {
		m_mouse_LButton_down = false;
		return true;
	}
	if (button_name == "PointerRight") {
		m_mouse_RButton_down = false;
		return true;
	}
	return false;
}

bool MovementController::VOnKeyDown(const BYTE c) {
	m_bKey[c] = true;
	return true;
}

bool MovementController::VOnKeyUp(const BYTE c) {
	m_bKey[c] = false;
	return true;
}

const DirectX::XMFLOAT4X4& MovementController::GetToWorld4x4() {
	return m_mat_to_world;
}

DirectX::XMMATRIX MovementController::GetToWorld() {
	return DirectX::XMLoadFloat4x4(&m_mat_to_world);
}

const DirectX::XMFLOAT4X4& MovementController::GetFromWorld4x4() {
	return m_mat_from_world;
}

DirectX::XMMATRIX MovementController::GetFromWorld() {
	return DirectX::XMLoadFloat4x4(&m_mat_from_world);
}
