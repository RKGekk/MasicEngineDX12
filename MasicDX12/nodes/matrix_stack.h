#pragma once

#include <deque>
#include <iostream>

#include <DirectXMath.h>

#include "../tools/string_utility.h"

class MatrixStack {
	std::deque<DirectX::XMFLOAT4X4> m_MatrixStack;

public:
	MatrixStack();

	void Push();
	void Pop();
	void MultMatrix(DirectX::FXMMATRIX other);
	void MultMatrix(const DirectX::XMFLOAT4X4& other);
	void MultMatrixLocal(DirectX::FXMMATRIX other);
	void MultMatrixLocal(const DirectX::XMFLOAT4X4& other);
	void LoadIdentity();
	void LoadMatrix(const DirectX::XMFLOAT4X4& other);
	void LoadMatrix(DirectX::FXMMATRIX other);
	const DirectX::XMFLOAT4X4& GetTop();
	void RotateAxis(const DirectX::XMFLOAT3& axis, float angle);
	void RotateAxis(DirectX::FXMVECTOR axis, float angle);
	void RotateAxisLocal(const DirectX::XMFLOAT3& axis, float angle);
	void RotateAxisLocal(const DirectX::FXMVECTOR& axis, float angle);
	void RotateYawPitchRoll(float yaw, float pitch, float roll);
	void RotateYawPitchRollLocal(float yaw, float pitch, float roll);
	void Scale(float x, float y, float z);
	void ScaleLocal(float x, float y, float z);
	void Translate(float x, float y, float z);
	void TranslateLocal(float x, float y, float z);

	friend std::ostream& operator<<(std::ostream& os, const MatrixStack& mstack);
};