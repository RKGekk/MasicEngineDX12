#include "light_node.h"

LightNode::LightNode(WeakBaseRenderComponentPtr renderComponent, const LightProperties& props, const DirectX::XMFLOAT4X4* t) : SceneNode(renderComponent, RenderPass::RenderPass_NotRendered, t) {
	m_light_props = props;
}

LightNode::LightNode(WeakBaseRenderComponentPtr renderComponent, const LightProperties& props, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from, bool calulate_from) : SceneNode(renderComponent, RenderPass::RenderPass_NotRendered, to, from, calulate_from) {
	m_light_props = props;
}

const LightProperties& LightNode::VGetLight() const {
	return m_light_props;
}