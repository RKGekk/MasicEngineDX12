#pragma once

#include <memory>
#include <vector>

#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "../graphics/constant_buffer_types.h"
#include "../graphics/shader.h"

class Scene;
class SceneNode;

class ShadowManager {
	friend class Scene;

protected:
	static const int SMapSize = 2048;
	void BuildShadowTransform(Scene* pScene);

public:
	ShadowManager();
	void CalcShadow(Scene* pScene);

	ID3D11ShaderResourceView* const* ShadowDepthMapSRVAddress() const;

	DirectX::XMMATRIX GetLightView();
	const DirectX::XMFLOAT4X4& GetLightView4x4();
	const DirectX::XMFLOAT4X4& GetLightView4x4T();

	DirectX::XMMATRIX GetLightProj();
	const DirectX::XMFLOAT4X4& GetLightProj4x4();
	const DirectX::XMFLOAT4X4& GetLightProj4x4T();

	DirectX::XMMATRIX GetShadowTransform();
	const DirectX::XMFLOAT4X4& GetShadowTransform4x4();
	const DirectX::XMFLOAT4X4& GetShadowTransform4x4T();

	DirectX::XMMATRIX GetWorldShadowTransform(Scene* pScene);
	DirectX::XMMATRIX GetWorldShadowTransform(DirectX::XMMATRIX world);
	const DirectX::XMFLOAT4X4& GetWorldShadowTransform4x4(Scene* pScene);
	const DirectX::XMFLOAT4X4& GetWorldShadowTransform4x4T(Scene* pScene);
	const DirectX::XMFLOAT4X4& GetWorldShadowTransform4x4T(DirectX::XMMATRIX world);

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	m_depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depth_map;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_map_dsv;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depth_map_srv;

	D3D11_VIEWPORT m_viewport;

	std::shared_ptr<VertexShader> m_vs;
	std::shared_ptr<PixelShader> m_ps;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;

	DirectX::XMFLOAT4X4 m_light_view;
	DirectX::XMFLOAT4X4 m_light_proj;
	DirectX::XMFLOAT4X4 m_shadow_transform;
};