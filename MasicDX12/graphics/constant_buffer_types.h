#pragma once

#include <DirectXMath.h>

#include "directional_light.h"
#include "point_light.h"
#include "spot_light.h"
#include "material_shader.h"

struct CB_VS_VertexShader {
	DirectX::XMFLOAT4X4 lwvpMatrix;
	DirectX::XMFLOAT4X4 invWorldMatrix;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 shadowTransform;
	MaterialShader material;
	DirectX::XMFLOAT4X4 gTexTransform;
};

struct CB_PS_PixelShader_Light {
	DirectionalLight gDirLights[3];
	//PointLight gPointLights[3];
	//SpotLight gSpotLights[3];

	DirectX::XMFLOAT3 gEyePosW;
	float Pad;

	DirectX::XMFLOAT4 gFogColor;
	DirectX::XMFLOAT4 gFogStartAndRange;
};

struct CB_PS_PixelShader_Light_All {
	DirectionalLight gDirLights[3];
	PointLight gPointLights[3];
	SpotLight gSpotLights[3];

	int gDirLightCount;
	int gPointLightCount;
	int gSpotLightCount;
	float gFogStart;

	DirectX::XMFLOAT3 gEyePosW;
	float gFogRange;

	DirectX::XMFLOAT4 gFogColor;

};

struct CB_VS_VertexShader_PerObject {
	DirectX::XMFLOAT4X4 worldMatrix;
};


struct CB_GS_GeometryShader_PerObject {
	DirectX::XMFLOAT4X4 gViewProj;
};

struct CB_GS_GeometryShader_PerPrame {
	DirectX::XMFLOAT4 gEyePosW;
	DirectX::XMFLOAT4 gAtlasDim;
};