#pragma once

#include <memory>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "scene_node.h"

class CameraNode : public SceneNode {
public:
	CameraNode(const std::string& name);
	CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform);
	CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const DirectX::XMFLOAT4X4& proj);
	CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform);
	CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj);

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnUpdate() override;

	void SetProjection(DirectX::FXMMATRIX proj);
	void SetProjection(const DirectX::XMFLOAT4X4& proj);

	DirectX::XMMATRIX GetWorldViewProjection(DirectX::FXMMATRIX world) const;
	DirectX::XMMATRIX GetWorldViewProjection(const DirectX::XMFLOAT4X4& world) const;
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4(DirectX::XMMATRIX world) const;
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4T(DirectX::XMMATRIX world) const;
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4(const DirectX::XMFLOAT4X4& world) const;
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4T(const DirectX::XMFLOAT4X4& world) const;

	DirectX::XMMATRIX GetViewProjection() const;
	DirectX::XMFLOAT4X4 GetViewProjection4x4() const;
	DirectX::XMFLOAT4X4 GetViewProjection4x4T() const;

	DirectX::XMMATRIX GetProjection() const;
	const DirectX::XMFLOAT4X4& GetProjection4x4f() const;
	DirectX::XMFLOAT4X4 GetProjection4x4fT() const;

	DirectX::XMMATRIX GetView() const;
	const DirectX::XMFLOAT4X4& GetView4x4f() const;
	DirectX::XMFLOAT4X4 GetView4x4fT() const;

protected:
	DirectX::XMFLOAT4X4 m_projection;
};