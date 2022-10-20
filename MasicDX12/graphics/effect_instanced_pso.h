#pragma once

#include <DirectXMath.h>

#include <memory>
#include <vector>
#include <cstdint>

#include "directx12_wrappers/shader.h"
#include "../tools/game_timer.h"

class CommandList;
class Device;
class Material;
class RootSignature;
class PipelineStateObject;
class ShaderResourceView;
class Texture;
class LightManager;
class ShadowManager;
class MeshManager;
class StructuredBuffer;
class BasicCameraNode;

class EffectInstancedPSO {
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

	struct alignas(16) PerPassData {
		DirectX::XMMATRIX ViewMatrix;
		DirectX::XMMATRIX InverseTransposeViewMatrix;

		DirectX::XMMATRIX ProjectionMatrix;
		DirectX::XMMATRIX InverseTransposeProjectionMatrix;

		DirectX::XMMATRIX ViewProjectionMatrix;
		DirectX::XMMATRIX InverseTransposeViewProjectionMatrix;

		float RenderTargetSizeX;
		float RenderTargetSizeY;
		float InverseRenderTargetSizeX;
		float InverseRenderTargetSizeY;

		float  NearZ;
		float  FarZ;
		float  TotalTime;
		float  DeltaTime;

		DirectX::XMMATRIX ShadowTransform;
	};

	enum class RootParameters {
		PerPassData,
		InstanceData,
		InstanceIndexData,
		MaterialCB,
		LightPropertiesCB,
		FogPropertiesCB,
		PointLights,
		SpotLights,
		DirectionalLights,
		Textures,
		NumRootParameters
	};

	EffectInstancedPSO(std::shared_ptr<Device> device);
	virtual ~EffectInstancedPSO();

	void SetLightManager(std::shared_ptr<LightManager> light_manager);
	void SetShadowManager(std::shared_ptr<ShadowManager> shadow_manager);
	void SetMeshManager(std::shared_ptr<MeshManager> mesh_manager);

	void SetFogProperties(const FogProperties& fog_props);
	void SetViewMatrix(const BasicCameraNode& camera);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX view_matrix);
	void XM_CALLCONV SetProjectionMatrix(DirectX::FXMMATRIX projection_matrix);
	void SetNearZ(float near_z);
	void SetFarZ(float far_z);
	void SetRenderTargetSize(DirectX::XMFLOAT2 render_target_size);
	void SetShadowMapTexture(std::shared_ptr<Texture> shadow_map_texture);
	

	void Apply(CommandList& command_list, const GameTimerDelta& delta);

private:
	enum DirtyFlags {
		DF_None              = 0,
		DF_PointLights       = (1 << 0),
		DF_SpotLights        = (1 << 1),
		DF_DirectionalLights = (1 << 2),
		DF_Material          = (1 << 3),
		DF_PerPassData       = (1 << 4),
		DF_InstanceData      = (1 << 5),
		DF_InstanceIndexData = (1 << 6),
		DF_RT_Size           = (1 << 7),
		DF_Near_Far          = (1 << 8),
		DF_All = DF_PointLights | DF_SpotLights | DF_DirectionalLights | DF_Material | DF_PerPassData | DF_InstanceData | DF_RT_Size | DF_Near_Far
	};

	struct alignas(16) VP {
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

	inline void BindTexture(CommandList& command_list, uint32_t offset, const std::shared_ptr<Texture>& texture);

	std::shared_ptr<Device> m_device;
	std::shared_ptr<RootSignature> m_root_signature;
	std::shared_ptr<PipelineStateObject> m_pipeline_state_object;

	std::shared_ptr<VertexShader> m_vertex_shader;
	std::shared_ptr<PixelShader> m_pixel_shader;

	DXGI_FORMAT m_back_buffer_format;
	DXGI_FORMAT m_depth_buffer_format;
	DXGI_SAMPLE_DESC m_sample_desc;

	std::shared_ptr<ShadowManager> m_shadow_manager;
	std::shared_ptr<LightManager> m_light_manager;
	std::shared_ptr<MeshManager> m_mesh_manager;

	std::shared_ptr<ShaderResourceView> m_default_srv;
	std::shared_ptr<Texture> m_shadow_map_texture;
	FogProperties m_fog_properties;

	VP* m_pAligned_mvp;

	bool m_need_transpose;

	DirectX::XMFLOAT4 m_render_target_size;
	float m_near_z;
	float m_far_z;

	CommandList* m_pPrevious_command_list;

	uint32_t m_dirty_flags;
};
