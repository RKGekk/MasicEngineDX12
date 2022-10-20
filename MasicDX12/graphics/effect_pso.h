#pragma once

#include <DirectXMath.h>

#include <memory>
#include <vector>

#include "directx12_wrappers/shader.h"

class CommandList;
class Device;
class Material;
class RootSignature;
class PipelineStateObject;
class ShaderResourceView;
class Texture;
class LightManager;

class EffectPSO {
public:
	struct LightProperties {
		uint32_t NumPointLights;
		uint32_t NumSpotLights;
		uint32_t NumDirectionalLights;
	};

	struct FogProperties {
		DirectX::XMFLOAT4 FogColor;
		float FogStart;
		float FogRange;
	};

	struct alignas(16) Matrices {
		DirectX::XMMATRIX ModelMatrix;
		DirectX::XMMATRIX ModelViewMatrix;
		DirectX::XMMATRIX InverseTransposeModelViewMatrix;
		DirectX::XMMATRIX ModelViewProjectionMatrix;
		DirectX::XMMATRIX ShadowMatrix;
	};

	enum RootParameters {
		MatricesCB,
		MaterialCB,
		LightPropertiesCB,
		FogPropertiesCB,
		PointLights,
		SpotLights,
		DirectionalLights,
		Textures,
		NumRootParameters
	};

	EffectPSO(std::shared_ptr<Device> device, bool enable_ligting, bool enable_decal);
	virtual ~EffectPSO();

	void SetLightManager(std::shared_ptr<LightManager> light_manager);
	void SetMaterial(const std::shared_ptr<Material>& material);

	void SetFogProperties(const FogProperties& fog_props);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world_matrix);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX view_matrix);
	void XM_CALLCONV SetProjectionMatrix(DirectX::FXMMATRIX projection_matrix);
	void XM_CALLCONV SetShadowMatrix(DirectX::FXMMATRIX shadow_matrix);
	void AddShadowTexture(std::shared_ptr<Texture> shadow_texture);

	void Apply(CommandList& command_list);

private:
	enum DirtyFlags {
		DF_None              = 0,
		DF_PointLights       = (1 << 0),
		DF_SpotLights        = (1 << 1),
		DF_DirectionalLights = (1 << 2),
		DF_Material          = (1 << 3),
		DF_Matrices          = (1 << 4),
		DF_All = DF_PointLights | DF_SpotLights | DF_DirectionalLights | DF_Material | DF_Matrices
	};

	struct alignas(16) MVP {
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMMATRIX Shadow;
	};

	inline void BindTexture(CommandList& command_list, uint32_t offset, const std::shared_ptr<Texture>& texture);

	std::shared_ptr<Device> m_device;
	std::shared_ptr<RootSignature> m_root_signature;
	std::shared_ptr<PipelineStateObject> m_pipeline_state_object;

	std::shared_ptr<VertexShader> m_vertex_shader;
	std::shared_ptr<PixelShader> m_pixel_shader;

	std::shared_ptr<LightManager> m_light_manager;

	std::shared_ptr<Material> m_material;
	FogProperties m_fog_properties;

	std::shared_ptr<ShaderResourceView> m_default_srv;

	MVP* m_pAligned_mvp;
	bool m_need_transpose;

	bool m_enable_lighting;
	bool m_enable_decal;

	CommandList* m_pPrevious_command_list;

	uint32_t m_dirty_flags;
};
