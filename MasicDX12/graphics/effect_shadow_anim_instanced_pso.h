#pragma once

#include <DirectXMath.h>

#include <memory>
#include <vector>
#include <cstdint>

#include "directx12_wrappers/shader.h"
#include "../tools/game_timer.h"

class CommandList;
class Device;
class RootSignature;
class PipelineStateObject;
class ShaderResourceView;
class SkinnedMeshManager;
class ShadowManager;
class StructuredBuffer;
class ShadowCameraNode;

class EffectAnimShadowInstancedPSO {
public:

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
	};

	struct alignas(16) FinalBoneTransforms {
		DirectX::XMMATRIX BoneTransforms[96];
		DirectX::XMMATRIX InverseTransposeBoneTransforms[96];
	};

	enum class RootParameters {
		PerPassData,
		InstanceData,
		InstanceIndexData,
		FinalBoneTransformsData,
		NumRootParameters
	};

	EffectAnimShadowInstancedPSO(std::shared_ptr<Device> device, std::shared_ptr<ShadowManager> shadow_manager);
	virtual ~EffectAnimShadowInstancedPSO();

	void SetSkinnedMeshManager(std::shared_ptr<SkinnedMeshManager> skinned_mesh_manager);

	void SetFinalBoneTransforms(const std::vector<DirectX::XMFLOAT4X4>& final_transforms_matrix);
	void SetViewMatrix(const ShadowCameraNode& camera);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX view_matrix);
	void XM_CALLCONV SetProjectionMatrix(DirectX::FXMMATRIX projection_matrix);
	void SetNearZ(float near_z);
	void SetFarZ(float far_z);
	void SetRenderTargetSize(DirectX::XMFLOAT2 render_target_size);

	void Apply(CommandList& command_list, const GameTimerDelta& delta);

private:
	enum DirtyFlags {
		DF_None = 0,
		DF_PerPassData = (1 << 0),
		DF_InstanceData = (1 << 1),
		DF_InstanceIndexData = (1 << 2),
		DF_RT_Size = (1 << 3),
		DF_Near_Far = (1 << 4),
		DF_FinalBoneTransforms = (1 << 5),
		DF_All = DF_PerPassData | DF_InstanceData | DF_RT_Size | DF_Near_Far
	};

	struct alignas(16) VP {
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

	std::shared_ptr<Device> m_device;
	std::shared_ptr<RootSignature> m_root_signature;
	std::shared_ptr<PipelineStateObject> m_pipeline_state_object;

	std::shared_ptr<VertexShader> m_vertex_shader;
	std::shared_ptr<PixelShader> m_pixel_shader;

	std::shared_ptr<SkinnedMeshManager> m_skinned_mesh_manager;
	std::shared_ptr<ShadowManager> m_shadow_manager;

	std::shared_ptr<ShaderResourceView> m_default_srv;

	VP* m_pAligned_mvp;
	FinalBoneTransforms* m_pAligned_fbt;

	bool m_need_transpose;

	DirectX::XMFLOAT4 m_render_target_size;
	float m_near_z;
	float m_far_z;

	CommandList* m_pPrevious_command_list;

	uint32_t m_dirty_flags;
};