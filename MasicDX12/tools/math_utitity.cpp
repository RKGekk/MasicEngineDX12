#include  "math_utitity.h"

DirectX::XMVECTOR GetTranslation(DirectX::XMMATRIX mat) {
	return mat.r[3];
}

DirectX::XMVECTOR BarycentricToVec3(DirectX::FXMVECTOR v0, DirectX::FXMVECTOR v1, DirectX::FXMVECTOR v2, float u, float v) {
	using namespace DirectX;
	return v0 + u * (v1 - v0) + v * (v2 - v0);
}

DirectX::XMVECTOR BarycentricToVec3(const DirectX::XMFLOAT3& v0, const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, float u, float v) {
	return BarycentricToVec3(DirectX::XMLoadFloat3(&v0), DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2), u, v);
}

DirectX::XMVECTOR BarycentricToVec3(const DirectX::XMFLOAT4& v0, const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2, float u, float v) {
	return BarycentricToVec3(DirectX::XMLoadFloat4(&v0), DirectX::XMLoadFloat4(&v1), DirectX::XMLoadFloat4(&v2), u, v);
}