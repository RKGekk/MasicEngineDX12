#include "matrix_stack.h"

MatrixStack::MatrixStack() {
	DirectX::XMFLOAT4X4 identity;
	DirectX::XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());
	m_MatrixStack.push_back(identity);
}

void MatrixStack::Push() {
	m_MatrixStack.push_back(m_MatrixStack.back());
}

void MatrixStack::Pop() {
	if (!m_MatrixStack.empty()) {
		m_MatrixStack.pop_back();
	}
}

void MatrixStack::MultMatrix(DirectX::FXMMATRIX other) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixMultiply(currentMatrix, other));
}

void MatrixStack::MultMatrix(const DirectX::XMFLOAT4X4& other) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMMATRIX otherMatrix = DirectX::XMLoadFloat4x4(&other);
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixMultiply(currentMatrix, otherMatrix));
}

void MatrixStack::MultMatrixLocal(DirectX::FXMMATRIX other) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixMultiply(other, currentMatrix));
}

void MatrixStack::MultMatrixLocal(const DirectX::XMFLOAT4X4& other) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMMATRIX otherMatrix = DirectX::XMLoadFloat4x4(&other);
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixMultiply(otherMatrix, currentMatrix));
}

void MatrixStack::LoadIdentity() {
	DirectX::XMFLOAT4X4 identity;
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixIdentity());
}

void MatrixStack::LoadMatrix(const DirectX::XMFLOAT4X4& other) {
	m_MatrixStack.back() = other;
}

void MatrixStack::LoadMatrix(DirectX::FXMMATRIX other) {
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), other);
}

const DirectX::XMFLOAT4X4& MatrixStack::GetTop() {
	return m_MatrixStack.back();
}

void MatrixStack::RotateAxis(const DirectX::XMFLOAT3& axis, float angle) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMVECTOR axis3f = DirectX::XMLoadFloat3(&axis);
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixRotationAxis(axis3f, angle)
		)
	);
}

void MatrixStack::RotateAxis(DirectX::FXMVECTOR axis, float angle) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixRotationAxis(axis, angle)
		)
	);
}

void MatrixStack::RotateAxisLocal(const DirectX::XMFLOAT3& axis, float angle) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMVECTOR axis3f = DirectX::XMLoadFloat3(&axis);
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationAxis(axis3f, angle),
			currentMatrix
		)
	);
}

void MatrixStack::RotateAxisLocal(const DirectX::FXMVECTOR& axis, float angle) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationAxis(axis, angle),
			currentMatrix
		)
	);
}

void MatrixStack::RotateYawPitchRoll(float yaw, float pitch, float roll) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll)
		)
	);
}

void MatrixStack::RotateYawPitchRollLocal(float yaw, float pitch, float roll) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll),
			currentMatrix
		)
	);
}

void MatrixStack::Scale(float x, float y, float z) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixScaling(x, y, z)
		)
	);
}

void MatrixStack::ScaleLocal(float x, float y, float z) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixScaling(x, y, z),
			currentMatrix
		)
	);
}

void MatrixStack::Translate(float x, float y, float z) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixTranslation(x, y, z)
		)
	);
}

void MatrixStack::TranslateLocal(float x, float y, float z) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixTranslation(x, y, z),
			currentMatrix
		)
	);
}

std::ostream& operator<<(std::ostream& os, const MatrixStack& mstack) {
	std::ios::fmtflags oldFlag = os.flags();
	int counter = 0;
	for (const auto& m : mstack.m_MatrixStack) {
		os << ++counter << ") Matrix number" << std::endl;
		os << m;
	}
	os.flags(oldFlag);
	return os;
}