#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <Windows.h>

#include "scene_node_properties.h"
#include "visitor.h"

class Scene;
class ISceneNode;

class ISceneNode {
public:
	virtual const SceneNodeProperties& VGet() const = 0;

	virtual void Accept(Visitor& visitor) = 0;

	virtual void VSetTransform(DirectX::FXMMATRIX to_world, DirectX::CXMMATRIX from_world, bool calulate_from) = 0;
	virtual void VSetTransform4x4(const DirectX::XMFLOAT4X4* to_world, const DirectX::XMFLOAT4X4* from_world) = 0;
	virtual DirectX::XMMATRIX VGetTransform() = 0;
	virtual DirectX::XMFLOAT4X4 VGetTransform4x4() = 0;
	virtual DirectX::XMFLOAT4X4 VGetTransform4x4T() = 0;

	virtual HRESULT VOnUpdate(float elapsed_seconds) = 0;
	virtual HRESULT VOnRestore() = 0;
	virtual HRESULT VOnLostDevice() = 0;

	virtual bool VAddChild(std::shared_ptr<ISceneNode> kid) = 0;
	virtual bool VRemoveChild(std::shared_ptr<ISceneNode>) = 0;
	virtual std::shared_ptr<ISceneNode> VGetParent() = 0;

	virtual ~ISceneNode() {};
};