#pragma once

#include <DirectXMath.h>

#include <memory>
#include <vector>

#include "directx12_wrappers/shader.h"

class CommandList;
class Device;
class RootSignature;
class PipelineStateObject;
class ShaderResourceView;
class ShadowManager;

class EffectShadowAnimPSO {
public:
	struct alignas(16) Matrices {
		DirectX::XMMATRIX ModelMatrix;
		DirectX::XMMATRIX ModelViewMatrix;
		DirectX::XMMATRIX InverseTransposeModelViewMatrix;
		DirectX::XMMATRIX ModelViewProjectionMatrix;
	};

	struct alignas(16) FinalBoneTransforms {
		DirectX::XMMATRIX BoneTransforms[96];
		DirectX::XMMATRIX InverseTransposeBoneTransforms[96];
	};

	enum RootParameters {
		MatricesCB,
		FinalBoneTransformsCB,
		NumRootParameters
	};

	EffectShadowAnimPSO(std::shared_ptr<Device> device, std::shared_ptr<ShadowManager> shadow_manager);
	virtual ~EffectShadowAnimPSO();

	void SetFinalBoneTransforms(const std::vector<DirectX::XMFLOAT4X4>& final_transforms_matrix);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world_matrix);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX view_matrix);
	void XM_CALLCONV SetProjectionMatrix(DirectX::FXMMATRIX projection_matrix);

	void Apply(CommandList& command_list);

private:
	enum DirtyFlags {
		DF_None = 0,
		DF_Matrices = (1 << 0),
		DF_FinalBoneTransforms = (1 << 1),
		DF_All = DF_Matrices
	};

	struct alignas(16) MVP {
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

	std::shared_ptr<Device> m_device;
	std::shared_ptr<RootSignature> m_root_signature;
	std::shared_ptr<PipelineStateObject> m_pipeline_state_object;

	std::shared_ptr<ShadowManager> m_shadow_manager;

	std::shared_ptr<VertexShader> m_vertex_shader;
	std::shared_ptr<PixelShader> m_pixel_shader;

	std::shared_ptr<ShaderResourceView> m_default_srv;

	MVP* m_pAligned_mvp;
	FinalBoneTransforms* m_pAligned_fbt;
	bool m_need_transpose;

	CommandList* m_pPrevious_command_list;

	uint32_t m_dirty_flags;
};