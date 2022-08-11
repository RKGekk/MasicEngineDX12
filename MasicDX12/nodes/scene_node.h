#pragma once

#include <memory>
#include <vector>

#include <DirectXMath.h>

#include "scene_node_properties.h"
#include "visitor.h"
#include "../tools/memory_utility.h"

class SceneNode;
using SceneNodeList = std::vector<std::shared_ptr<SceneNode>>;

class SceneNode : public std::enable_shared_from_this<SceneNode> {
	friend class Scene;

protected:
	SceneNodeList m_children;
	std::weak_ptr<SceneNode> m_pParent;
	std::weak_ptr<SceneNode> m_pRoot;
	SceneNodeProperties m_props;

public:
	SceneNode(const std::string& name, const DirectX::XMFLOAT4X4* to, const DirectX::XMFLOAT4X4* from = nullptr);
	SceneNode(const std::string& name, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from, bool calulate_from = true);

	virtual ~SceneNode();

	virtual void Accept(Visitor& visitor);
	virtual bool VIsVisible() const = 0;
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnUpdate();
	virtual HRESULT VOnLostDevice();

	virtual bool VAddChild(std::shared_ptr<SceneNode> kid);
	virtual bool VRemoveChild(std::shared_ptr<SceneNode> cid);

	const SceneNodeProperties& Get() const;

	void UpdateCumulativeTransform();
	void UpdateCumulativeScale();
	void SetTransform4x4(const DirectX::XMFLOAT4X4* toWorld, const DirectX::XMFLOAT4X4* fromWorld);
	void SetTransform(DirectX::FXMMATRIX toWorld, DirectX::CXMMATRIX fromWorld, bool calulate_from);

	void SetParent(std::shared_ptr<SceneNode> parent_node);
	std::shared_ptr<SceneNode> GetParent();

	void SetAlpha(float alpha);
	void SetName(std::string name);
	void SetPosition3(const DirectX::XMFLOAT3& pos);

	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetScale(DirectX::XMVECTOR scale);
	void SetMaterial(const Material& mat);	
};