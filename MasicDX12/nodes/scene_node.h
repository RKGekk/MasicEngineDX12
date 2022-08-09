#pragma once

#include <memory>

#include <DirectXMath.h>

#include "../actors/actor.h"
#include "i_scene_node.h"
#include "../actors/base_render_component.h"
#include "../tools/memory_utility.h"

using SceneNodeList = std::vector<std::shared_ptr<ISceneNode>>;

class SceneNode : public ISceneNode {
	friend class Scene;

protected:
	SceneNodeList m_children;
	std::weak_ptr<SceneNode> m_pParent;
	SceneNodeProperties m_props;

public:
	SceneNode(const std::string& name, const DirectX::XMFLOAT4X4* to, const DirectX::XMFLOAT4X4* from = nullptr);
	SceneNode(const std::string& name, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from, bool calulate_from = true);

	virtual ~SceneNode();

	virtual const SceneNodeProperties& VGet() const override;

	virtual void VSetTransform4x4(const DirectX::XMFLOAT4X4* toWorld, const DirectX::XMFLOAT4X4* fromWorld) override;
	virtual void VSetTransform(DirectX::FXMMATRIX toWorld, DirectX::CXMMATRIX fromWorld, bool calulate_from) override;
	virtual DirectX::XMMATRIX VGetTransform() override;
	virtual DirectX::XMFLOAT4X4 VGetTransform4x4() override;
	virtual DirectX::XMFLOAT4X4 VGetTransform4x4T() override;

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnUpdate() override;

	virtual bool VAddChild(std::shared_ptr<ISceneNode> kid) override;
	virtual bool VRemoveChild(std::shared_ptr<ISceneNode> cid) override;
	virtual HRESULT VOnLostDevice() override;

	virtual std::shared_ptr<ISceneNode> VGetParent() override;

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