#pragma once

#pragma once

#include <memory>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "scene_node.h"
#include "camera_node.h"

class BasicCameraNode : public CameraNode {
public:
	BasicCameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, float fovy, float aspect, float near_clip, float far_clip);
	BasicCameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const DirectX::XMFLOAT4X4& proj);
	BasicCameraNode(const std::string& name, const DirectX::BoundingFrustum& frustum);
	BasicCameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, float fovy, float aspect, float near_clip, float far_clip);
	BasicCameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj);

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnUpdate() override;

	const DirectX::BoundingFrustum& GetFrustum() const;
	void UpdateFrustum();
	void SetFovYRad(float fovy);
	void SetFovYDeg(float fovy);
	float GetFovYRad() const;
	float GetFovYDeg() const;
	void SetNear(float near_cut);
	void SetFar(float far_cut);

	void SetProjection(const DirectX::BoundingFrustum& frustum);
	void SetProjection(float fovy, float aspect, float near_clip, float far_clip);

protected:
	void SetData(DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj);
	void SetData(DirectX::BoundingFrustum frustum);

private:
	DirectX::BoundingFrustum m_frustum;
	float m_fovy;
	float m_aspect;
};