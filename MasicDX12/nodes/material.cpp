#include "material.h"

Material::Material() {
	ZeroMemory(&m_D3DMaterial, sizeof(MaterialDX));
	m_D3DMaterial.Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_D3DMaterial.Ambient = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	m_D3DMaterial.Specular = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_D3DMaterial.Emissive = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Material::SetAmbient(DirectX::FXMVECTOR color) {
	DirectX::XMStoreFloat3(&m_D3DMaterial.Ambient, color);
}

void Material::SetAmbient(DirectX::XMFLOAT3 color) {
	m_D3DMaterial.Ambient = color;
}

DirectX::XMVECTOR Material::GetAmbient() const {
	return DirectX::XMLoadFloat3(&m_D3DMaterial.Ambient);
}

const DirectX::XMFLOAT3& Material::GetAmbient3() const {
	return m_D3DMaterial.Ambient;
}

void Material::SetDiffuse(DirectX::FXMVECTOR color) {
	DirectX::XMStoreFloat4(&m_D3DMaterial.Diffuse, color);
}

void Material::SetDiffuse(DirectX::XMFLOAT4 color) {
	m_D3DMaterial.Diffuse = color;
}

DirectX::XMVECTOR Material::GetDiffuse() const {
	return DirectX::XMLoadFloat4(&m_D3DMaterial.Diffuse);
}

const DirectX::XMFLOAT4& Material::GetDiffuse4() const {
	return m_D3DMaterial.Diffuse;
}

void Material::SetSpecular(DirectX::FXMVECTOR color, const float power) {
	DirectX::XMStoreFloat3(&m_D3DMaterial.Specular, color);
	m_D3DMaterial.Power = power;
}

void Material::SetSpecular(DirectX::XMFLOAT3 color, const float power) {
	m_D3DMaterial.Specular = color;
	m_D3DMaterial.Power = power;
}

DirectX::XMVECTOR Material::GetSpecular() const {
	return DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&m_D3DMaterial.Ambient), m_D3DMaterial.Power);
}

const DirectX::XMFLOAT3& Material::GetSpecular3() const {
	return m_D3DMaterial.Specular;
}

void Material::SetEmissive(DirectX::FXMVECTOR color) {
	DirectX::XMStoreFloat3(&m_D3DMaterial.Emissive, color);
}

void Material::SetEmissive(DirectX::XMFLOAT3 color) {
	m_D3DMaterial.Emissive = color;
}

DirectX::XMVECTOR Material::GetEmissive() const {
	return DirectX::XMLoadFloat3(&m_D3DMaterial.Emissive);
}

const DirectX::XMFLOAT3& Material::GetEmissive3() const {
	return m_D3DMaterial.Emissive;
}

void Material::SetAlpha(const float alpha) {
	m_D3DMaterial.Diffuse.w = alpha;
}

bool Material::HasAlpha() const {
	return GetAlpha() != 1.0f;
}

float Material::GetAlpha() const {
	return m_D3DMaterial.Diffuse.w;
}