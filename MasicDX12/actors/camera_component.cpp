#include "camera_component.h"

#include "../tools/string_utility.h"
#include "../nodes/camera_node.h"
#include "../application.h"

const std::string CameraComponent::g_Name = "CameraComponent";

CameraComponent::CameraComponent() : m_fov(90.0f), m_near(0.1f), m_far(100.0f) {}

CameraComponent::CameraComponent(const pugi::xml_node& data) : m_fov(90.0f), m_near(0.1f), m_far(100.0f) {
	Init(data);
}

bool CameraComponent::VInit(const pugi::xml_node& data) {
	return Init(data);
}

const std::string& CameraComponent::VGetName() const {
	return CameraComponent::g_Name;
}

pugi::xml_node CameraComponent::VGenerateXml() {
	return pugi::xml_node();
}

std::shared_ptr<SceneNode> CameraComponent::VGetSceneNode() {
	return m_scene_node;
}

bool CameraComponent::Init(const pugi::xml_node& data) {
	std::string name = GetOwner()->GetName();
	m_fov = data.child("Fov").text().as_float(m_fov);
	m_near = data.child("Near").text().as_float(m_near);
	m_far = data.child("Far").text().as_float(m_far);
	float aspect_ratio = Application::Get().GetApplicationOptions().GetAspect();

	m_scene_node = std::make_shared<CameraNode>(name, DirectX::XMMatrixIdentity(), m_fov, aspect_ratio, m_near, m_far);

	return true;
}
