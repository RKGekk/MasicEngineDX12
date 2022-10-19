#pragma once

#include <memory>
#include <vector>

#include <DirectXMath.h>

#include "scene_node_properties.h"
#include "i_visitor.h"
#include "../tools/memory_utility.h"

class SceneNode;
using SceneNodeList = std::vector<std::shared_ptr<SceneNode>>;

class SceneNode : public std::enable_shared_from_this<SceneNode> {
friend class Scene;
public:
	SceneNode(const std::string& name);
	SceneNode(const std::string& name, uint32_t group_id);
	SceneNode(const std::string& name, const DirectX::XMFLOAT4X4* to, const DirectX::XMFLOAT4X4* from = nullptr);
	SceneNode(const std::string& name, uint32_t group_id, const DirectX::XMFLOAT4X4* to, const DirectX::XMFLOAT4X4* from = nullptr);
	SceneNode(const std::string& name, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from = DirectX::XMMatrixIdentity(), bool calulate_from = true);
	SceneNode(const std::string& name, uint32_t group_id, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from = DirectX::XMMatrixIdentity(), bool calulate_from = true);

	virtual ~SceneNode();

	virtual void Accept(IVisitor& visitor);
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnUpdate();
	virtual HRESULT VOnLostDevice();

	virtual bool VAddChild(std::shared_ptr<SceneNode> kid);
	virtual bool VRemoveChild(std::shared_ptr<SceneNode> cid);
	virtual const SceneNodeList& VGetChildren() const;

	const SceneNodeProperties& Get() const;

	void UpdateMergedAABB();

	void UpdateCumulativeTransform();
	void UpdateCumulativeScale();

	void SetTransform4x4(const DirectX::XMFLOAT4X4* toWorld, const DirectX::XMFLOAT4X4* fromWorld);
	void SetTransform(DirectX::FXMMATRIX toWorld, DirectX::CXMMATRIX fromWorld = DirectX::XMMatrixIdentity(), bool calulate_from = true);

	void SetPosition(DirectX::XMVECTOR pos);
	void SetPosition3(const DirectX::XMFLOAT3& pos);
	void SetPosition4(const DirectX::XMFLOAT4& pos);

	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetScale(DirectX::XMVECTOR scale);

	void SetParent(std::shared_ptr<SceneNode> parent_node);
	std::shared_ptr<SceneNode> GetParent();

	void SetName(std::string name);

	void SetDirtyFlags(uint32_t flags);
	void AddDirtyFlags(uint32_t flags);
	void RemoveDirtyFlags(uint32_t flags);

	void SetGroupID(uint32_t id);

	void SetAABB(const DirectX::BoundingBox& aabb);

protected:
	SceneNodeList m_children;
	std::weak_ptr<SceneNode> m_pParent;
	SceneNodeProperties m_props;
};