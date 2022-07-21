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
	//float3 pixelColor = objTexture.Sample(objSamplerState, input.uv);
	//return float4(pixelColor, 1.0f);
	input.normal = normalize(input.normal);
	float3 sampleColor = objTexture.Sample(objSamplerState, input.uv);
	//float3 sampleColor = input.normal;
	float3 ambientLight = ambientLightColor * ambientLightStrength;
	
	float3 appliedLight = ambientLight;
	
	float distanceToLight = distance(dLightPos, input.worldPos);
	float att = 1 / (dAtt_a + dAtt_b * distanceToLight + dAtt_c * distanceToLight * distanceToLight);
	float3 vectorToLight = normalize(dLightPos - input.worldPos);
	float diffuseLightIntensity = max(dot(vectorToLight, input.normal), 0.0f) * att;
	float3 diffuseLight = diffuseLightIntensity * dLightStrength * dLightColor;
	
	appliedLight += diffuseLight;
	
	float3 finalColor = appliedLight * sampleColor;
	//return float4(input.uv.x, input.uv.y, 0.0f, 1.0f);
	return float4(sampleColor, 1.0f);
	//return float4(finalColor, 1.0f);
	//return float4(pixelColor, alpha);
	//return float4(input.color, 1.0f);
	
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
}