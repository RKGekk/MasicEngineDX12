struct PS_INPUT {
	float4 position : SV_POSITION;
    float3 PosL     : POSITION;
};

TextureCube gCubeMap : TEXTURE1 : register(t0);
SamplerState CubeMapSamplerState : SAMPLER1 : register(s0);

float4 main(PS_INPUT input) : SV_TARGET {
	
	//float Brightness = -0.2f;
    //float Contrast = 1.3f;
    float Brightness = 0.0f;
    float Contrast = 1.0f;
    float4 pixelColor = gCubeMap.Sample(CubeMapSamplerState, input.PosL);

    pixelColor.rgb = ((pixelColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f;
    pixelColor.rgb += Brightness;

    return pixelColor;
}