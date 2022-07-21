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
	float4 position : SV_POSITION;
    float3 PosL : POSITION;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

    output.position = mul(float4(input.pos, 1.0f), lwvpMatrix);;
    output.position = output.position.xyww;
    output.PosL = float3(input.pos.x, input.pos.y, input.pos.z);

    return output;
}