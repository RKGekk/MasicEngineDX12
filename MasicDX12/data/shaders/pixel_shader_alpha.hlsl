cbuffer lightBuffer : register(b0) {
	float3 ambientLightColor;
	float ambientLightStrength;
	float3 dLightColor;
	float dLightStrength;
	float3 dLightPos;
	float dAtt_a;
	float dAtt_b;
	float dAtt_c;
};

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 worldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET {
	float4 sampleColor = objTexture.Sample(objSamplerState, input.uv);
	clip(sampleColor.a - 0.05f);
	return sampleColor;
}