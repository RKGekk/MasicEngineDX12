#include "base_render_component.h"

#include <algorithm>
#include <functional>
#include <cctype>

bool BaseRenderComponent::VInit(const pugi::xml_node& data) {
	m_active = true;

	/*TiXmlElement* pActive = pData->FirstChildElement("Active");
	if (pActive) {
		std::string sActive = pActive->FirstChild()->Value();
		std::for_each(sActive.begin(), sActive.end(), [](char& c) { c = ::toupper(c); });
		if (sActive == "TRUE" || sActive == "1") {
			m_active = true;
		}
		else {
			m_active = false;
		}
	}

	TiXmlElement* pColorNode = pData->FirstChildElement("Color");
	if (pColorNode) {
		m_color = LoadColor(pColorNode);
	}*/

	return VDelegateInit(data);
}

void BaseRenderComponent::VPostInit() {
	std::shared_ptr<SceneNode> pSceneNode(VGetSceneNode());
	std::shared_ptr<EvtData_New_Render_Component> pEvent(new EvtData_New_Render_Component(m_pOwner->GetId(), VGetId(), pSceneNode));
	IEventManager::Get()->VTriggerEvent(pEvent);
}

void BaseRenderComponent::VOnChanged() {
	std::shared_ptr<EvtData_Modified_Render_Component> pEvent(new EvtData_Modified_Render_Component(m_pOwner->GetId(), VGetId()));
	IEventManager::Get()->VTriggerEvent(pEvent);
}

pugi::xml_node BaseRenderComponent::VGenerateXml() {
	pugi::xml_node pBaseElement = VCreateBaseElement();
	return pBaseElement;
}

void BaseRenderComponent::VRegisterEvents() {}

const DirectX::XMFLOAT4& BaseRenderComponent::GetColor() const {
	return m_color;
}

void BaseRenderComponent::SetColor(DirectX::XMFLOAT4 color) {
	m_color = color;
}

void BaseRenderComponent::SetColorR(float color) {
	m_color.x = color;
}

void BaseRenderComponent::SetColorG(float color) {
	m_color.y = color;
}

void BaseRenderComponent::SetColorB(float color) {
	m_color.z = color;
}

void BaseRenderComponent::SetColorA(float color) {
	m_color.w = color;
}

bool BaseRenderComponent::VDelegateInit(const pugi::xml_node& data) {
	return true;
}

DirectX::XMFLOAT4 BaseRenderComponent::LoadColor(const pugi::xml_node& data) {
	DirectX::XMFLOAT4 color;

	double r = 1.0;
	double g = 1.0;
	double b = 1.0;
	double a = 1.0;

	/*pData->Attribute("r", &r);
	pData->Attribute("g", &g);
	pData->Attribute("b", &b);
	pData->Attribute("a", &a);*/

	color.x = (float)r;
	color.y = (float)g;
	color.z = (float)b;
	color.w = (float)a;

	return color;
}

pugi::xml_node BaseRenderComponent::VCreateBaseElement() {
	return pugi::xml_node();
}

std::shared_ptr<SceneNode> BaseRenderComponent::VGetSceneNode() {
	if (!m_pSceneNode) {
		m_pSceneNode = VCreateSceneNode();
	}
	return m_pSceneNode;
}