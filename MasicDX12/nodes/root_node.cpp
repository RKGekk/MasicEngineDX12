#include "root_node.h"
#include "scene.h"

#include "../engine/i_render_state.h"
#include "../graphics/i_renderer.h"

RootNode::RootNode() : SceneNode(WeakBaseRenderComponentPtr(), RenderPass::RenderPass_0, nullptr) {
	m_Children.reserve((unsigned)RenderPass::RenderPass_Last + 1u);

	std::shared_ptr<SceneNode> staticGroup(new SceneNode(WeakBaseRenderComponentPtr(), RenderPass::RenderPass_Static, nullptr));
	m_Children.push_back(staticGroup);

	std::shared_ptr<SceneNode> actorGroup(new SceneNode(WeakBaseRenderComponentPtr(), RenderPass::RenderPass_Actor, nullptr));
	m_Children.push_back(actorGroup);

	std::shared_ptr<SceneNode> skyGroup(new SceneNode(WeakBaseRenderComponentPtr(), RenderPass::RenderPass_Sky, nullptr));
	m_Children.push_back(skyGroup);

	std::shared_ptr<SceneNode> invisibleGroup(new SceneNode(WeakBaseRenderComponentPtr(), RenderPass::RenderPass_NotRendered, nullptr));
	m_Children.push_back(invisibleGroup);
}

bool RootNode::VAddChild(std::shared_ptr<ISceneNode> kid) {
	RenderPass pass = kid->VGet().RenderPass();
	if ((unsigned)pass >= m_Children.size() || !m_Children[(unsigned)pass]) {
		return false;
	}

	return m_Children[(unsigned)pass]->VAddChild(kid);
}

HRESULT RootNode::VRenderChildren(Scene* pScene) {
	for (int pass = (int)RenderPass::RenderPass_0; pass < (int)RenderPass::RenderPass_Last; ++pass) {
		switch ((RenderPass)pass) {
		case RenderPass::RenderPass_Static:
		case RenderPass::RenderPass_Actor:
		{
			m_Children[pass]->VRenderChildren(pScene);
		}
		break;
		case RenderPass::RenderPass_Sky:
		{
			//std::shared_ptr<IRenderState> skyPass = pScene->GetRenderer()->VPrepareSkyBoxPass();
			m_Children[pass]->VRenderChildren(pScene);
		}
		break;
		}
	}

	return S_OK;
}

bool RootNode::VRemoveChild(ActorId aid, ComponentId cid) {
	bool anythingRemoved = false;
	for (int i = (int)RenderPass::RenderPass_0; i < (int)RenderPass::RenderPass_Last; ++i) {
		if (m_Children[i]->VRemoveChild(aid, cid)) {
			anythingRemoved = true;
		}
	}
	return anythingRemoved;
}

bool RootNode::VIsVisible(Scene* pScene) const {
	return true;
}

ISceneNode* RootNode::GetRenderPass(RenderPass pass) {
	return m_Children[(int)pass].get();
}
