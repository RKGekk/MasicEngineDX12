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
	float4x4 gTexTransform;
};

struct VS_INPUT {
	float3 pos : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tg : TANGENT;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tg : TANGENT;
	float3 worldPos : WORLD_POSITION;
	float4 ShadowPosH : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;
	output.pos = mul(float4(input.pos, 1.0f), lwvpMatrix);
	output.color = input.color;
	output.uv = mul(float4(input.uv, 0.0f, 1.0f), gTexTransform).xy;
	output.uv.x += gTexTransform[0][3];
	output.uv.y += gTexTransform[1][3];
	//output.uv = input.uv;
	output.normal = mul(float4(input.normal, 0.0f), invWorldMatrix);
	output.normal = normalize(output.normal);
	output.tg = mul(float4(input.tg, 0.0f), invWorldMatrix);
	output.worldPos = mul(float4(input.pos, 1.0f), worldMatrix);
	output.ShadowPosH = mul(float4(input.pos, 1.0f), shadowTransform);
	return output;
}