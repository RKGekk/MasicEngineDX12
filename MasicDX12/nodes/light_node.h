#pragma once

#include <memory>

#include <DirectXMath.h>

#include "scene_node.h"
#include "../actors/base_render_component.h"
#include "light_properties.h"

class LightNode;
typedef std::list<std::shared_ptr<LightNode>> Lights;

class LightNode : public SceneNode {
protected:
	LightProperties m_light_props;

public:
	LightNode(WeakBaseRenderComponentPtr renderComponent, const LightProperties& props, const DirectX::XMFLOAT4X4* t);
	LightNode(WeakBaseRenderComponentPtr renderComponent, const LightProperties& props, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from, bool calulate_from = false);

	const LightProperties& VGetLight() const;
};