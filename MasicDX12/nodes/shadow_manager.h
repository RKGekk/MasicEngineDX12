#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <d3d12.h>
#include "../graphics/directx12_wrappers/render_target.h"

class SceneNode;
class ShadowCameraNode;

class ShadowManager {
	friend class Scene;
public:
	using ShadowName = std::string;
	using ShadowMap = std::unordered_map<ShadowName, std::shared_ptr<ShadowCameraNode>>;

	ShadowManager();

	void AddShadow(std::shared_ptr<SceneNode> node);
	void RemoveMesh(std::shared_ptr<SceneNode> node);

	const ShadowMap& GetShadowMap() const;
	int GetShadowCount(ShadowName shadow_name);
	std::shared_ptr<ShadowCameraNode> GetShadow(ShadowName shadow_name);
	std::shared_ptr<ShadowCameraNode> GetShadow();
	DXGI_FORMAT GetShadowBufferFormat();
	std::shared_ptr<Texture> GetShadowMapTexture();
	D3D12_VIEWPORT GetShadowViewport();
	D3D12_RECT GetShadowRect();
	std::shared_ptr<RenderTarget> GetRT();

protected:
	void MakeRT();

	ShadowMap m_shadow_map;
	DXGI_FORMAT m_shadow_buffer_format;
	std::shared_ptr<RenderTarget> m_shadow_rt;
	//std::shared_ptr<Texture> m_shadow_map_texture;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissor_rect;
};