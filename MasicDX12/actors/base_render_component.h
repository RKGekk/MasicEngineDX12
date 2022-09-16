#pragma once

#include <memory>
#include <string>

#include <DirectXMath.h>
#include <pugixml/pugixml.hpp>

#include "render_component_interface.h"
#include "../events/evt_data_new_render_component.h"
#include "../events/evt_data_modified_render_component.h"
#include "../events/i_event_manager.h"

class BaseRenderComponent;
typedef BaseRenderComponent* WeakBaseRenderComponentPtr;

class BaseRenderComponent : public RenderComponentInterface {
protected:
    virtual void VRegisterEvents();

    DirectX::XMFLOAT4 m_color;
    std::shared_ptr<SceneNode> m_pSceneNode;
    bool m_active;

public:
    virtual bool VInit(const pugi::xml_node& data) override;
    virtual void VPostInit() override;
    virtual void VOnChanged() override;
    virtual pugi::xml_node VGenerateXml() override;

    const DirectX::XMFLOAT4& GetColor() const;
    void SetColor(DirectX::XMFLOAT4 color);
    void SetColorR(float color);
    void SetColorG(float color);
    void SetColorB(float color);
    void SetColorA(float color);

protected:
    virtual bool VDelegateInit(const pugi::xml_node& data);
    virtual std::shared_ptr<SceneNode> VCreateSceneNode() = 0;
    DirectX::XMFLOAT4 LoadColor(const pugi::xml_node& data);

    virtual pugi::xml_node VCreateBaseElement();
    virtual void VCreateInheritedXmlElements(pugi::xml_node& data) = 0;

public:
    virtual std::shared_ptr<SceneNode> VGetSceneNode() override;
};