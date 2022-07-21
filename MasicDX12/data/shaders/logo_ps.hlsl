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
	//float Brightness = 0.1f;
    //float Contrast = 1.3f;
	float3 sampleColor = objTexture.Sample(objSamplerState, input.uv);
	
	//sampleColor.rgb = ((sampleColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f;
    //sampleColor.rgb += Brightness;
	
	return float4(sampleColor * input.color, 1.0f);
}