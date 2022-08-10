#pragma once

#include <memory>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "scene_node.h"

class CameraNode : public SceneNode {
public:
	CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, float fovy, float aspect, float near_clip, float far_clip);
	CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const DirectX::XMFLOAT4X4& proj);
	CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, float fovy, float aspect, float near_clip, float far_clip);
	CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj);

	virtual HRESULT VOnRestore() override;

	const DirectX::BoundingFrustum& GetFrustum() const;
	void SetFovYRad(float fovy);
	void SetFovYDeg(float fovy);
	float GetFovYRad();
	float GetFovYDeg();

	DirectX::XMMATRIX GetWorldViewProjection(DirectX::FXMMATRIX world);
	DirectX::XMMATRIX GetWorldViewProjection(const DirectX::XMFLOAT4X4& world);
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4(DirectX::XMMATRIX world);
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4T(DirectX::XMMATRIX world);
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4(const DirectX::XMFLOAT4X4& world);
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4T(const DirectX::XMFLOAT4X4& world);

	DirectX::XMMATRIX GetViewProjection();
	DirectX::XMFLOAT4X4 GetViewProjection4x4();
	DirectX::XMFLOAT4X4 GetViewProjection4x4T();

	DirectX::XMMATRIX GetProjection();
	const DirectX::XMFLOAT4X4& GetProjection4x4f();
	DirectX::XMFLOAT4X4 GetProjection4x4fT();

	DirectX::XMMATRIX GetView();
	const DirectX::XMFLOAT4X4& GetView4x4f();
	DirectX::XMFLOAT4X4 GetView4x4fT();

protected:
	void SetData(DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj);

	DirectX::BoundingFrustum m_frustum;
	DirectX::XMFLOAT4X4 m_projection;
	float m_fovy;
	float m_aspect;
};