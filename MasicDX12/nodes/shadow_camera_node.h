#pragma once

#include <memory>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "scene_node.h"
#include "camera_node.h"
#include "light_node.h"

class ShadowCameraNode : public CameraNode {
public:
	struct ShadowCameraProps {
		int ShadowMapWidth;
		int ShadowMapHeight;
		int DepthBias;
		float DepthBiasClamp;
		float SlopeScaledDepthBias;
	};

	ShadowCameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const std::shared_ptr<SceneNode> root_node, const std::shared_ptr<LightNode> light_node, ShadowCameraProps camera_props);
	ShadowCameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, const std::shared_ptr<SceneNode> root_node, const std::shared_ptr<LightNode> light_node, ShadowCameraProps camera_props);

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnUpdate() override;

	const DirectX::BoundingOrientedBox& GetFrustum() const;
	void UpdateShadowTransform();
	
	void SetProjection(const DirectX::BoundingOrientedBox& frustum);

	DirectX::XMMATRIX GetShadowTranform() const;
	const DirectX::XMFLOAT4X4& GetShadowTranform4x4() const;
	DirectX::XMFLOAT4X4 GetShadowTranform4x4T() const;

	const ShadowCameraProps& GetShadowProps() const;

protected:
	DirectX::BoundingOrientedBox m_frustum;
	DirectX::XMFLOAT4X4 m_shadow_transform;

	std::weak_ptr<SceneNode> m_root_node;
	std::weak_ptr<LightNode> m_light_node;

	ShadowCameraProps m_shadow_camera_props;
};