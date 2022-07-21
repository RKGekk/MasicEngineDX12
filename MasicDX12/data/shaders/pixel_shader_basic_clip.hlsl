struct DirectionalLight {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

struct Material {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

cbuffer perObjectBuffer : register(b0) {
	float4x4 lwvpMatrix;
	float4x4 invWorldMatrix;
	float4x4 worldMatrix;
	float4x4 shadowTransform;
	Material gMaterial;
};

cbuffer cbPerFrame : register(b1) {
	DirectionalLight gDirLights[3];
	PointLight gPointLights[3];
	SpotLight gSpotLights[3];
	
	int gDirLightCount;
	int gPointLightCount;
	int gSpotLightCount;
	float  gFogStart;
	
	float3 gEyePosW;
	float  gFogRange;
	
	float4 gFogColor; 
};

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tg : TANGENT;
	float3 worldPos : WORLD_POSITION;
	float4 ShadowPosH : TEXCOORD1;
};

Texture2D gDiffuseMap : TEXTURE1 : register(t0);
SamplerState DiffuseMapSamplerState : SAMPLER1 : register(s0);

Texture2D gNormalMap : TEXTURE2 : register(t1);
SamplerState NormalMapSamplerState : SAMPLER2 : register(s1);

Texture2D gShadowMap : TEXTURE3 : register(t2);
//SamplerState ShadowMapSamplerState : SAMPLER3 : register(s2);
SamplerComparisonState ShadowMapSamplerState : SAMPLER3 : register(s2);

float4 main(PS_INPUT input) : SV_TARGET {
	
	float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	texColor = gDiffuseMap.Sample(DiffuseMapSamplerState, input.uv);
	clip(texColor.a - 0.05f);

    return texColor;
}