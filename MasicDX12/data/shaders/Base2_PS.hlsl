struct PixelShaderInput {
	float4 PositionVS  : POSITION;
	float3 NormalVS    : NORMAL;
	float3 TangentVS   : TANGENT;
	float3 BitangentVS : BITANGENT;
	float2 TexCoordTS  : TEXCOORD;
    float4 PositionHS  : SV_Position;
    float4 ShadowPosHS : SHADOW;
    //float4 PositionWS  : POSITION2;
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

struct BlinnPhongSpecMaterial {
    float3 FresnelR0;
    float Shininess;
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

float3 LinearToSRGB(float3 color) {
    // This is exactly the sRGB curve
    //return color < 0.0031308f ? 12.92f * color : 1.055f * pow(abs(color), 1.0f / 2.4f) - 0.055f;

    // This is cheaper but nearly equivalent
	return color < 0.0031308f ? 12.92f * color : 1.13005f * sqrt(abs(color - 0.00228f)) - 0.13448f * color + 0.005719f;
}

#if ENABLE_LIGHTING
float DoDiffuse(float3 n, float3 L) {
    return max(0.0f, dot(n, L));
}

float DoSpecular(float3 v, float3 n, float3 L, float specular_power) {
    float3 r = normalize(reflect(-L, n));
    float r_dot_v = max(0.0f, dot(r, v));

    return pow(r_dot_v, specular_power);
}

float CalcLinearAttenuation(float d, float falloff_start, float falloff_end) {
    // Linear falloff.
    return saturate((falloff_end - d) / (falloff_end - falloff_start));
}

float DoAttenuation(float c, float l, float q, float d) {
    return 1.0f / ( c + l * d + q * d * d );
}

float DoSpotCone(float3 spot_dir, float3 L, float spot_angle) {
    float min_cos = cos(spot_angle);
    float max_cos = (min_cos + 1.0f) / 2.0f;
    float cos_angle = dot(spot_dir, -L);
    return smoothstep(min_cos, max_cos, cos_angle);
}

// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
float3 SchlickFresnel(float3 R0, float3 normal, float3 light_direction_normal_ws) {
    float cos_incident_angle = saturate(dot(normal, light_direction_normal_ws));

    float f0 = 1.0f - cos_incident_angle;
    float3 reflect_percent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

    return reflect_percent;
}

float3 BlinnPhongSpec(float3 light_direction_normal_ws, float3 normal, float3 toEye, BlinnPhongSpecMaterial mat) {
    const float m = mat.Shininess;
    float3 half_vec = normalize(toEye + light_direction_normal_ws);

    float roughness_factor = (m + 8.0f) * pow(max(dot(half_vec, normal), 0.0f), m) / 8.0f;
    float3 fresnel_factor = SchlickFresnel(mat.FresnelR0, half_vec, light_direction_normal_ws);

    float3 spec_albedo = fresnel_factor * roughness_factor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    spec_albedo = spec_albedo / (spec_albedo + 1.0f);

    return spec_albedo;
}

LightResult DoPointLightVS(PointLight light, BlinnPhongSpecMaterial pbr, float3 v, float3 p, float3 n) {
    LightResult result;
    
    float3 L = (light.PositionVS.xyz - p);
    float d = length(L);
    L = L / d;

    float attenuation = DoAttenuation(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, d);

    result.Diffuse = light.Color * DoDiffuse(n, L) * attenuation;
    //result.Specular = light.Color * DoSpecular(V, N, L, pbr.Shininess) * attenuation;
    result.Specular = light.Color * float4(BlinnPhongSpec(L, n, v, pbr), 0.0f) * attenuation;
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoSpotLightVS(SpotLight light, BlinnPhongSpecMaterial pbr, float3 v, float3 p, float3 n) {
    LightResult result;
    
    float3 L = (light.PositionVS.xyz - p);
    float d = length(L);
    L = L / d;

    float attenuation = DoAttenuation(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, d);
    float spot_intensity = DoSpotCone(light.DirectionVS.xyz, L, light.SpotAngle);

    result.Diffuse = light.Color * DoDiffuse(n, L) * attenuation * spot_intensity;
    //result.Specular = DoSpecular(V, N, L, pbr.Shininess) * attenuation * spot_intensity * light.Color;
    result.Specular = light.Color * float4(BlinnPhongSpec(L, n, v, pbr), 0.0f) * attenuation * spot_intensity;
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoDirectionalLightVS(DirectionalLight light, BlinnPhongSpecMaterial pbr, float3 v, float3 p, float3 n) {
    LightResult result;

    float3 L = normalize(-light.DirectionVS.xyz);

    result.Diffuse = light.Color * DoDiffuse(n, L);
    //result.Specular = light.Color * DoSpecular(v, n, L, pbr.Shininess);
    result.Specular = light.Color * float4(BlinnPhongSpec(L, n, v, pbr), 0.0f);
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoLighting(BlinnPhongSpecMaterial pbr, float3 p, float3 n) {
    uint i;

    // Lighting is performed in view space.
    float3 v = normalize(-p);

    LightResult total_result = (LightResult)0;

    // Iterate point lights.
    for (i = 0; i < LightPropertiesCB.NumPointLights; ++i) {
        LightResult result = DoPointLightVS(PointLights[i], pbr, v, p, n);

        total_result.Diffuse += result.Diffuse;
        total_result.Specular += result.Specular;
        total_result.Ambient += result.Ambient;
    }

    // Iterate spot lights.
    for (i = 0; i < LightPropertiesCB.NumSpotLights; ++i) {
        LightResult result = DoSpotLightVS(SpotLights[i], pbr, v, p, n);

        total_result.Diffuse += result.Diffuse;
        total_result.Specular += result.Specular;
        total_result.Ambient += result.Ambient;
    }

    // Iterate directinal lights
    for (i = 0; i < LightPropertiesCB.NumDirectionalLights; ++i) {
        LightResult result = DoDirectionalLightVS(DirectionalLights[i], pbr, v, p, n);

        total_result.Diffuse += result.Diffuse;
        total_result.Specular += result.Specular;
        total_result.Ambient += result.Ambient;
    }

    total_result.Diffuse = saturate(total_result.Diffuse);
    total_result.Specular = saturate(total_result.Specular);
    total_result.Ambient = saturate(total_result.Ambient);

    return total_result;
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

float3 DoBumpMapping(float3x3 TBN, Texture2D tex, float2 uv, float bump_scale) {
    // Sample the heightmap at the current texture coordinate.
	float height_00 = tex.Sample(TextureSampler, uv).r * bump_scale;
    // Sample the heightmap in the U texture coordinate direction.
	float height_10 = tex.Sample(TextureSampler, uv, int2(1, 0)).r * bump_scale;
    // Sample the heightmap in the V texture coordinate direction.
	float height_01 = tex.Sample(TextureSampler, uv, int2(0, 1)).r * bump_scale;

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
    float2 texture_uv = IN.TexCoordTS.xy;

    // By default, use the alpha component of the diffuse color.
	float alpha = material.Diffuse.a;
	if (material.HasTexture & HAS_OPACITY_TEXTURE) {
		alpha = OpacityTexture.Sample(TextureSampler, texture_uv).r;
	}

#if ENABLE_DECAL
    if ( alpha < 0.1f ) {
        discard; // Discard the pixel if it is below a certain threshold.
    }
#endif // ENABLE_DECAL

	float4 ambient = material.Ambient;
	if (material.HasTexture & HAS_AMBIENT_TEXTURE) {
		ambient = SampleTexture(AmbientTexture, texture_uv, ambient);
	}
    
    float4 emissive = material.Emissive;
	if (material.HasTexture & HAS_EMISSIVE_TEXTURE) {
		emissive = SampleTexture(EmissiveTexture, texture_uv, emissive);
	}
    
    float4 diffuse_albedo = material.Diffuse;
	if (material.HasTexture & HAS_DIFFUSE_TEXTURE) {
		diffuse_albedo = SampleTexture(DiffuseTexture, texture_uv, diffuse_albedo);
	}
    
    float specular_power = material.SpecularPower;
	if (material.HasTexture & HAS_SPECULAR_POWER_TEXTURE) {
		specular_power *= SpecularPowerTexture.Sample(TextureSampler, texture_uv).r;
	}
    
    float k = (material.IndexOfRefraction - 1.0f) / (material.IndexOfRefraction + 1.0f);
    float k2 = k * k;
    float3 fresnelR0 = max(k2, material.Reflectance).xyz;
    if (material.HasTexture & HAS_METALNESS_TEXTURE) {
        float3 metalness_sample = MetalnessTexture.Sample(TextureSampler, texture_uv).rgb;
		fresnelR0 = lerp(fresnelR0, diffuse_albedo.rgb, metalness_sample);
	}

	float3 n;
    // Normal mapping
	if (material.HasTexture & HAS_NORMAL_TEXTURE) {
		float3 tangent = normalize(IN.TangentVS);
		float3 bitangent = normalize(IN.BitangentVS);
		float3 normal = normalize(IN.NormalVS);

		float3x3 TBN = float3x3(tangent, bitangent, normal);

		n = DoNormalMapping(TBN, NormalTexture, texture_uv);
	}
	else if (material.HasTexture & HAS_BUMP_TEXTURE) {
		float3 tangent = normalize(IN.TangentVS);
		float3 bitangent = normalize(IN.BitangentVS);
		float3 normal = normalize(IN.NormalVS);

		float3x3 TBN = float3x3(tangent, -bitangent, normal);

		n = DoBumpMapping(TBN, BumpTexture, texture_uv, material.BumpIntensity);
	}
	else {
		n = normalize(IN.NormalVS);
	}

	float shadow = 1.0f;
    if (material.HasTexture & HAS_SHADOW_TEXTURE) {
        shadow = CalcShadowFactor(IN.ShadowPosHS);
	}
    
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
#if ENABLE_LIGHTING
	BlinnPhongSpecMaterial pbr = (BlinnPhongSpecMaterial)0.0f;
    pbr.FresnelR0 = fresnelR0;
    pbr.Shininess = specular_power;
    
    LightResult lit = DoLighting(pbr, IN.PositionVS.xyz, n);
    ambient = ambient * 0.5f + diffuse_albedo * lit.Ambient * 0.5f;
    diffuse_albedo *= lit.Diffuse;
    // Specular power less than 1 doesn't really make sense.
    // Ignore specular on materials with a specular power less than 1.
    if (material.SpecularPower > 1.0f) {
        specular = material.Specular;
        if (material.HasTexture & HAS_SPECULAR_TEXTURE) {
            specular = SampleTexture(SpecularTexture, texture_uv, specular);
        }
        specular *= lit.Specular;
    }
#else 
	shadow = -n.z;
#endif // ENABLE_LIGHTING
    
    //float3 eye_position_w = IN.PositionVS - IN.PositionWS;
    //float3 to_eye = eye_position_w - IN.PositionWS.xyz;
    //float distance_to_eye = length(to_eye);
    float distance_to_eye = length(IN.PositionVS.xyz);

    float4 result = float4(ambient.rgb + (emissive + diffuse_albedo + specular).rgb * shadow, alpha * material.Opacity);
    
    float fog_start = FogPropertiesCB.FogStart;
    float fog_range = FogPropertiesCB.FogRange;
    float3 fog_color = FogPropertiesCB.FogColor;
    float fog_lerp = saturate((distance_to_eye - fog_start) / fog_range);
    result = float4(lerp(result.xyz, fog_color, fog_lerp), result.w);
    
    return float4(LinearToSRGB(result.xyz), result.a);
	//return float4(ambient.rgb + (emissive + diffuse_albedo + specular).rgb * shadow, alpha * material.Opacity);
}