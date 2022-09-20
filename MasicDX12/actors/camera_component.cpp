#include "camera_component.h"

#include "../tools/string_utility.h"
#include "../nodes/camera_node.h"
#include "../application.h"
#include "transform_component.h"

const std::string CameraComponent::g_Name = "CameraComponent";

CameraComponent::CameraComponent() : m_fov(90.0f), m_near(0.1f), m_far(100.0f), m_aspect_ratio(1.0f) {}

CameraComponent::CameraComponent(const pugi::xml_node& data) : m_fov(90.0f), m_near(0.1f), m_far(100.0f), m_aspect_ratio(1.0f) {
	VDelegateInit(data);
}

void CameraComponent::VDelegatePostInit() {
	std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
	scene_node->VAddChild(m_loaded_scene_node);
}

void CameraComponent::VDelegateUpdate(const GameTimerDelta& delta) {}

const std::string& CameraComponent::VGetName() const {
	return CameraComponent::g_Name;
}

pugi::xml_node CameraComponent::VGenerateXml() {
	return pugi::xml_node();
}

std::shared_ptr<CameraNode> CameraComponent::VGetCameraNode() {
	return m_loaded_scene_node;
}

float CameraComponent::GetFov() {
	return m_fov;
}

void CameraComponent::SetFov(float fov) {
	m_fov = fov;
	m_loaded_scene_node->SetFovYRad(fov);
}

float CameraComponent::GetNear() {
	return m_near;
}

void CameraComponent::SetNear(float near_cut) {
	m_near = near_cut;
	m_loaded_scene_node->SetNear(near_cut);
}

float CameraComponent::GetFar() {
	return m_far;
}

void CameraComponent::SetFar(float far_cut) {
	m_far = far_cut;
	m_loaded_scene_node->SetNear(far_cut);
}

bool CameraComponent::VDelegateInit(const pugi::xml_node& data) {
	using namespace std::literals;

	m_fov = DirectX::XMConvertToRadians(data.child("Fov").text().as_float(m_fov));
	m_near = data.child("Near").text().as_float(m_near);
	m_far = data.child("Far").text().as_float(m_far);
	m_aspect_ratio = Application::Get().GetApplicationOptions().GetAspect();

	m_loaded_scene_node = std::make_shared<CameraNode>("CameraComponent"s, DirectX::XMMatrixIdentity(), m_fov, m_aspect_ratio, m_near, m_far);

	return true;
}
