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
	SceneNodeProperties m_props;

public:
	SceneNode(const std::string& name, const DirectX::XMFLOAT4X4* to, const DirectX::XMFLOAT4X4* from = nullptr);
	SceneNode(const std::string& name, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from, bool calulate_from = true);

	virtual ~SceneNode();

	virtual const SceneNodeProperties& VGet() const;

	virtual void Accept(Visitor& visitor);

	virtual void VSetTransform4x4(const DirectX::XMFLOAT4X4* toWorld, const DirectX::XMFLOAT4X4* fromWorld);
	virtual void VSetTransform(DirectX::FXMMATRIX toWorld, DirectX::CXMMATRIX fromWorld, bool calulate_from);
	virtual DirectX::XMMATRIX VGetTransform();
	virtual DirectX::XMFLOAT4X4 VGetTransform4x4();
	virtual DirectX::XMFLOAT4X4 VGetTransform4x4T();

	virtual HRESULT VOnRestore();
	virtual HRESULT VOnUpdate();

	virtual bool VAddChild(std::shared_ptr<SceneNode> kid);
	virtual bool VRemoveChild(std::shared_ptr<SceneNode> cid);
	virtual HRESULT VOnLostDevice();

	virtual std::shared_ptr<SceneNode> VGetParent();

	void SetAlpha(float alpha);
	float GetAlpha() const;

	void SetName(std::string name);
	const std::string& GetName() const;

	DirectX::XMVECTOR GetPosition() const;
	DirectX::XMFLOAT3 GetPosition3() const;
	DirectX::XMFLOAT4 GetPosition4() const;
	void SetPosition3(const DirectX::XMFLOAT3& pos);

	DirectX::XMFLOAT3 GetWorldPosition3() const;
	DirectX::XMVECTOR GetWorldPosition() const;

	DirectX::XMVECTOR GetDirection() const;
	DirectX::XMFLOAT3 GetDirection3f() const;
	DirectX::XMVECTOR GetUp() const;
	DirectX::XMFLOAT3 GetUp3f() const;

	const DirectX::XMFLOAT3& GetScale3f() const;
	DirectX::XMVECTOR GetScale() const;
	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetScale(DirectX::XMVECTOR scale);
	void SetMaterial(const Material& mat);

private:
	void SetTransform(DirectX::FXMMATRIX toWorld, DirectX::CXMMATRIX fromWorld, bool calulate_from);
	void SetTransform4x4(const DirectX::XMFLOAT4X4* toWorld, const DirectX::XMFLOAT4X4* fromWorld);
};