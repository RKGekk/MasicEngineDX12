struct PixelShaderInput {
	float4 PositionVS  : POSITION;
	float3 NormalVS    : NORMAL;
	float3 TangentVS   : TANGENT;
	float3 BitangentVS : BITANGENT;
	float2 TexCoord    : TEXCOORD;
    float4 Position    : SV_Position;
    float4 ShadowPosHS : SHADOW;
};

struct Material {
	float4 Diffuse;
    //------------------------------------ ( 16 bytes )
	float4 Specular;
    //------------------------------------ ( 16 bytes )
	float4 Emissive;
    //------------------------------------ ( 16 bytes )
	float4 Ambient;
    //------------------------------------ ( 16 bytes )
	float4 Reflectance;
    //------------------------------------ ( 16 bytes )
	float Opacity; // If Opacity < 1, then the material is transparent.
	float SpecularPower;
	float IndexOfRefraction; // For transparent materials, IOR > 0.
	float BumpIntensity; // When using bump textures (height maps) we need
                              // to scale the height values so the normals are visible.
    //------------------------------------ ( 16 bytes )
	uint HasTexture;
	uint Padding1;
	uint Padding2;
	uint Padding3;
    //------------------------------------ ( 16 bytes )
    // Total:                              ( 16 * 7 = 112 bytes )
};

#if ENABLE_LIGHTING
struct PointLight {
    float4 PositionWS; // Light position in world space.
    //----------------------------------- (16 byte boundary)
    float4 PositionVS; // Light position in view space.
    //----------------------------------- (16 byte boundary)
    float4 Color;
    //----------------------------------- (16 byte boundary)
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    float Padding1;
    //----------------------------------- (16 byte boundary)
    float3 Ambient;
    float Padding2;
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 5 = 80 bytes
};

struct SpotLight {
    float4 PositionWS; // Light position in world space.
    //----------------------------------- (16 byte boundary)
    float4 PositionVS; // Light position in view space.
    //----------------------------------- (16 byte boundary)
    float4 DirectionWS; // Light direction in world space.
    //----------------------------------- (16 byte boundary)
    float4 DirectionVS; // Light direction in view space.
    //----------------------------------- (16 byte boundary)
    float4 Color;
    //----------------------------------- (16 byte boundary)
    float  SpotAngle;
    float  ConstantAttenuation;
    float  LinearAttenuation;
    float  QuadraticAttenuation;
    //----------------------------------- (16 byte boundary)
    float3 Ambient;
    float Padding;
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 7 = 112 bytes
};

struct DirectionalLight {
    float4 DirectionWS;  // Light direction in world space.
    //----------------------------------- (16 byte boundary)
    float4 DirectionVS;  // Light direction in view space.
    //----------------------------------- (16 byte boundary)
    float4 Color;
    //----------------------------------- (16 byte boundary)
    float3 Ambient;
    float Padding;
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 4 = 64 bytes
};

struct LightProperties {
    uint NumPointLights;
    uint NumSpotLights;
    uint NumDirectionalLights;
};

struct LightResult {
    float4 Diffuse;
    float4 Specular;
    float4 Ambient;
};

ConstantBuffer<LightProperties> LightPropertiesCB : register(b1);

StructuredBuffer<PointLight> PointLights : register(t0);
StructuredBuffer<SpotLight> SpotLights : register(t1);
StructuredBuffer<DirectionalLight> DirectionalLights : register(t2);
#endif // ENABLE_LIGHTING

struct FogProperties {
    float4 FogColor;
    float  FogStart;
    float  FogRange;
};

ConstantBuffer<Material> MaterialCB           : register(b0, space1);
ConstantBuffer<FogProperties> FogPropertiesCB : register(b2);

// Textures
Texture2D AmbientTexture       : register(t3);
Texture2D EmissiveTexture      : register(t4);
Texture2D DiffuseTexture       : register(t5);
Texture2D SpecularTexture      : register(t6);
Texture2D SpecularPowerTexture : register(t7); // aiTextureType_SHININESS->roughness
Texture2D NormalTexture        : register(t8);
Texture2D BumpTexture          : register(t9);
Texture2D OpacityTexture       : register(t10);
Texture2D DisplacementTexture  : register(t11);
Texture2D MetalnessTexture     : register(t12);
Texture2D ShadowTexture        : register(t13);

SamplerState TextureSampler          : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

float3 LinearToSRGB(float3 x) {
    // This is exactly the sRGB curve
    //return x < 0.0031308 ? 12.92 * x : 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055;

    // This is cheaper but nearly equivalent
	return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(abs(x - 0.00228)) - 0.13448 * x + 0.005719;
}

#if ENABLE_LIGHTING
float DoDiffuse(float3 N, float3 L) {
    return max(0, dot(N, L));
}

float DoSpecular(float3 V, float3 N, float3 L, float specularPower) {
    float3 R = normalize(reflect(-L, N));
    float RdotV = max(0, dot(R, V));

    return pow(RdotV, specularPower);
}

float DoAttenuation(float c, float l, float q, float d) {
    return 1.0f / ( c + l * d + q * d * d );
}

float DoSpotCone(float3 spotDir, float3 L, float spotAngle) {
    float minCos = cos(spotAngle);
    float maxCos = (minCos + 1.0f) / 2.0f;
    float cosAngle = dot(spotDir, -L);
    return smoothstep(minCos, maxCos, cosAngle);
}

LightResult DoPointLight(PointLight light, float3 V, float3 P, float3 N, float specularPower) {
    LightResult result;
    float3 L = (light.PositionVS.xyz - P);
    float d = length(L);
    L = L / d;

    float attenuation = DoAttenuation(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, d);

    result.Diffuse = DoDiffuse(N, L) * attenuation * light.Color;
    result.Specular = DoSpecular(V, N, L, specularPower) * attenuation * light.Color;
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoSpotLight(SpotLight light, float3 V, float3 P, float3 N, float specularPower) {
    LightResult result;
    float3 L = (light.PositionVS.xyz - P);
    float d = length(L);
    L = L / d;

    float attenuation = DoAttenuation(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, d);

    float spotIntensity = DoSpotCone(light.DirectionVS.xyz, L, light.SpotAngle);

    result.Diffuse = DoDiffuse(N, L) * attenuation * spotIntensity * light.Color;
    result.Specular = DoSpecular(V, N, L, specularPower) * attenuation * spotIntensity * light.Color;
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoDirectionalLight(DirectionalLight light, float3 V, float3 P, float3 N, float specularPower) {
    LightResult result;

    float3 L = normalize(-light.DirectionVS.xyz);

    result.Diffuse = light.Color * DoDiffuse(N, L);
    result.Specular = light.Color * DoSpecular(V, N, L, specularPower);
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoLighting(float3 P, float3 N, float specularPower) {
    uint i;

    // Lighting is performed in view space.
    float3 V = normalize(-P);

    LightResult totalResult = (LightResult)0;

    // Iterate point lights.
    for (i = 0; i < LightPropertiesCB.NumPointLights; ++i) {
        LightResult result = DoPointLight(PointLights[i], V, P, N, specularPower);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    // Iterate spot lights.
    for (i = 0; i < LightPropertiesCB.NumSpotLights; ++i) {
        LightResult result = DoSpotLight(SpotLights[i], V, P, N, specularPower);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    // Iterate directinal lights
    for (i = 0; i < LightPropertiesCB.NumDirectionalLights; ++i) {
        LightResult result = DoDirectionalLight(DirectionalLights[i], V, P, N, specularPower);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);
    totalResult.Ambient = saturate(totalResult.Ambient);

    return totalResult;
}
#endif // ENABLE_LIGHTING

float3 ExpandNormal(float3 n) {
	return n * 2.0f - 1.0f;
}

float3 DoNormalMapping(float3x3 TBN, Texture2D tex, float2 uv) {
	float3 N = tex.Sample(TextureSampler, uv).xyz;
	N = ExpandNormal(N);

    // Transform normal from tangent space to view space.
	N = mul(N, TBN);
	return normalize(N);
}

float3 DoBumpMapping(float3x3 TBN, Texture2D tex, float2 uv, float bumpScale) {
    // Sample the heightmap at the current texture coordinate.
	float height_00 = tex.Sample(TextureSampler, uv).r * bumpScale;
    // Sample the heightmap in the U texture coordinate direction.
	float height_10 = tex.Sample(TextureSampler, uv, int2(1, 0)).r * bumpScale;
    // Sample the heightmap in the V texture coordinate direction.
	float height_01 = tex.Sample(TextureSampler, uv, int2(0, 1)).r * bumpScale;

	float3 p_00 = { 0, 0, height_00 };
	float3 p_10 = { 1, 0, height_10 };
	float3 p_01 = { 0, 1, height_01 };

    // normal = tangent x bitangent
	float3 tangent = normalize(p_10 - p_00);
	float3 bitangent = normalize(p_01 - p_00);

	float3 normal = cross(tangent, bitangent);

    // Transform normal from tangent space to view space.
	normal = mul(normal, TBN);

	return normal;
}


// If c is not black, then blend the color with the texture
// otherwise, replace the color with the texture.
float4 SampleTexture(Texture2D t, float2 uv, float4 c) {
	if (any(c.rgb)) {
		c *= t.Sample(TextureSampler, uv);
	}
	else {
		c = t.Sample(TextureSampler, uv);
	}

	return c;
}

float CalcShadowFactor(float4 shadow_pos_hs) {
    // Complete projection by doing division by w.
    shadow_pos_hs.xyz /= shadow_pos_hs.w;

    // Depth in NDC space.
    float depth = shadow_pos_hs.z;

    uint width;
    uint height;
    uint numMips;
    ShadowTexture.GetDimensions(0u, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float)width;

    float percent_lit = 0.0f;
    const float2 offsets[9] = {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for(int i = 0; i < 9; ++i) {
        percent_lit += ShadowTexture.SampleCmpLevelZero(ShadowSampler, shadow_pos_hs.xy + offsets[i], depth).r;
    }
    
    return percent_lit / 9.0f;
}


float4 main(PixelShaderInput IN) : SV_Target {
    const uint HAS_AMBIENT_TEXTURE        = 1u;
	const uint HAS_EMISSIVE_TEXTURE       = 2u;
	const uint HAS_DIFFUSE_TEXTURE        = 4u;
	const uint HAS_SPECULAR_TEXTURE       = 8u;
	const uint HAS_SPECULAR_POWER_TEXTURE = 16u; // aiTextureType_SHININESS->roughness
	const uint HAS_NORMAL_TEXTURE         = 32u;
	const uint HAS_BUMP_TEXTURE           = 64u;
	const uint HAS_OPACITY_TEXTURE        = 128u;
	const uint HAS_DISPLACEMENT_TEXTURE   = 256u;
    const uint HAS_METALNESS_TEXTURE      = 512u;
    const uint HAS_NORMAL_INV_Y_TEXTURE   = 1024u;
    const uint HAS_SHADOW_TEXTURE         = 2048u;
    
	Material material = MaterialCB;

    // By default, use the alpha component of the diffuse color.
	float alpha = material.Diffuse.a;
	if (material.HasTexture & HAS_OPACITY_TEXTURE) {
		alpha = OpacityTexture.Sample(TextureSampler, IN.TexCoord.xy).r;
	}

#if ENABLE_DECAL
    if ( alpha < 0.1f ) {
        discard; // Discard the pixel if it is below a certain threshold.
    }
#endif // ENABLE_DECAL

	float4 ambient = material.Ambient;
	float4 emissive = material.Emissive;
	float4 diffuse = material.Diffuse;
	float specularPower = material.SpecularPower;
	float2 uv = IN.TexCoord.xy;

	if (material.HasTexture & HAS_AMBIENT_TEXTURE) {
		ambient = SampleTexture(AmbientTexture, uv, ambient);
	}
    
	if (material.HasTexture & HAS_EMISSIVE_TEXTURE) {
		emissive = SampleTexture(EmissiveTexture, uv, emissive);
	}
    
	if (material.HasTexture & HAS_DIFFUSE_TEXTURE) {
		diffuse = SampleTexture(DiffuseTexture, uv, diffuse);
	}
    
	if (material.HasTexture & HAS_SPECULAR_POWER_TEXTURE) {
		specularPower *= SpecularPowerTexture.Sample(TextureSampler, uv).r;
	}

	float3 N;
    // Normal mapping
	if (material.HasTexture & HAS_NORMAL_TEXTURE) {
		float3 tangent = normalize(IN.TangentVS);
		float3 bitangent = normalize(IN.BitangentVS);
		float3 normal = normalize(IN.NormalVS);

		float3x3 TBN = float3x3(tangent, bitangent, normal);

		N = DoNormalMapping(TBN, NormalTexture, uv);
	}
	else if (material.HasTexture & HAS_BUMP_TEXTURE) {
		float3 tangent = normalize(IN.TangentVS);
		float3 bitangent = normalize(IN.BitangentVS);
		float3 normal = normalize(IN.NormalVS);

		float3x3 TBN = float3x3(tangent, -bitangent, normal);

		N = DoBumpMapping(TBN, BumpTexture, uv, material.BumpIntensity);
	}
	else {
		N = normalize(IN.NormalVS);
	}

	float shadow = 1.0f;
    if (material.HasTexture & HAS_SHADOW_TEXTURE) {
        shadow = CalcShadowFactor(IN.ShadowPosHS);
	}
    
	float4 specular = 0;
#if ENABLE_LIGHTING
    LightResult lit = DoLighting( IN.PositionVS.xyz, N, specularPower );
    ambient *= diffuse * lit.Ambient;
    diffuse *= lit.Diffuse;
    // Specular power less than 1 doesn't really make sense.
    // Ignore specular on materials with a specular power less than 1.
    if (material.SpecularPower > 1.0f) {
        specular = material.Specular;
        if (material.HasTexture & HAS_SPECULAR_TEXTURE) {
            specular = SampleTexture( SpecularTexture, uv, specular );
        }
        specular *= lit.Specular;
    }
#else 
	shadow = -N.z;
#endif // ENABLE_LIGHTING

	return float4(ambient.rgb + (emissive + diffuse + specular).rgb * shadow, alpha * material.Opacity);
}