cbuffer cbPerFrame : register(b0) {
	float4 gEyePosW;
	float4 gAtlasDim;
};

struct GeoOut {
	float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 Tex : TEXCOORD;
	float TTL : TTL;
    //uint PrimID : SV_PrimitiveID;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(GeoOut pin) : SV_Target {
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW.xyz - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
   
    // Default to multiplicative identity.
    float4 texColor = objTexture.Sample(objSamplerState, pin.Tex);
	//float4 out_color = float4(texColor.rrr, 1.0f);
	//float4 out_color = texColor.rrrr;
	//float4 out_color = float4(1.0f - texColor.r, 1.0f - texColor.r, 1.0f - texColor.r, 1.0f - texColor.r);
	//float4 out_color = float4(1.0f - texColor.r, 1.0f - texColor.r, 1.0f - texColor.r, texColor.r);
	//float4 out_color = float4(texColor.rrr * pin.TTL, texColor.r);
	float4 out_color = texColor.rrrr * pin.TTL;
	clip(out_color.a - 0.05f);

    return out_color;
}