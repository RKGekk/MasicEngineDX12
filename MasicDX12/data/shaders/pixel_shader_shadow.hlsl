struct PS_INPUT {
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D gDiffuseMap : TEXTURE1 : register(t0);
SamplerState DiffuseMapSamplerState : SAMPLER1 : register(s0);

float4 main(PS_INPUT input) : SV_TARGET {
	float4 texColor = gDiffuseMap.Sample(DiffuseMapSamplerState, input.Tex);
	//clip(texColor.a - 0.05f);
	//clip(texColor.a);
	return texColor;
}