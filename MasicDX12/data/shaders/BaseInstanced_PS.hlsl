struct PixelShaderInput {
	float4 PositionHS  : SV_Position;
	float4 PositionWS  : POSITION;
	float3 NormalWS    : NORMAL;
	float3 TangentWS   : TANGENT;
	float3 BitangentWS : BITANGENT;
	float2 TextureUV   : TEXCOORD;
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
	float BumpIntensity; // When using bump textures (height maps) we need to scale the height values so the normals are visible.
    //------------------------------------ ( 16 bytes )
	uint HasTexture;
	uint Padding1;
	uint Padding2;
	uint Padding3;
    //------------------------------------ ( 16 bytes )
    // Total:                              ( 16 * 7 = 112 bytes )
};

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

struct FogProperties {
    float4 FogColor;
    float  FogStart;
    float  FogRange;
};

struct PerPassData {
	matrix ViewMatrix;
	matrix InverseTransposeViewMatrix;
	
	matrix ProjectionMatrix;
	matrix InverseTransposeProjectionMatrix;
	
	matrix ViewProjectionMatrix;
	matrix InverseTransposeViewProjectionMatrix;
	
	float2 RenderTargetSize;
    float2 InverseRenderTargetSize;
    float  NearZ;
    float  FarZ;
    float  TotalTime;
    float  DeltaTime;
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

ConstantBuffer<PerPassData>        gPerPassData       : register(b0);
ConstantBuffer<Material>           gMaterialData      : register(b0, space1);
ConstantBuffer<LightProperties>    gLightPropertiesCB : register(b1);
ConstantBuffer<FogProperties>      FogPropertiesCB    : register(b2);

StructuredBuffer<PointLight>       PointLights        : register(t0);
StructuredBuffer<SpotLight>        SpotLights         : register(t1);
StructuredBuffer<DirectionalLight> DirectionalLights  : register(t2);

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
    //return x < 0.0031308 ? 12.92 * x : 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055;
}

float3 SRGBToLinear(float3 x) {
	return x < 0.04045f ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}

float DoDiffuse(float3 N, float3 L) {
    return max(0, dot(N, L));
}

float DoSpecular(float3 V, float3 N, float3 L, float specularPower) {
    float3 R = normalize(reflect(-L, N));
    float RdotV = max(0, dot(R, V));

    return pow(RdotV, specularPower);
}

float CalcLinearAttenuation(float d, float falloff_start, float falloff_end) {
    // Linear falloff.
    return saturate((falloff_end - d) / (falloff_end - falloff_start));
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

LightResult DoPointLightVS(PointLight light, float3 V, float3 P, float3 N, float specularPower) {
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

LightResult DoPointLightWS(PointLight light, BlinnPhongSpecMaterial pbr, float3 position_ws, float3 normal_ws, float3 to_eye_ws) {
    LightResult result;
    
    float3 light_vec = (light.PositionVS.xyz - position_ws);
    float d = length(light_vec);
    float3 light_direction_normal_ws = light_vec / d;

    float attenuation = DoAttenuation(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, d);

    result.Diffuse = light.Color * DoDiffuse(normal_ws, light_direction_normal_ws) * attenuation;
    result.Specular = light.Color * float4(BlinnPhongSpec(light_direction_normal_ws, normal_ws, to_eye_ws, pbr), 0.0f) * attenuation;
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoSpotLightVS(SpotLight light, float3 V, float3 P, float3 N, float specularPower) {
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

LightResult DoSpotLightWS(SpotLight light, BlinnPhongSpecMaterial pbr, float3 position_ws, float3 normal_ws, float3 to_eye_ws) {
    LightResult result;
    
    float3 light_vec = (light.PositionVS.xyz - position_ws);
    float d = length(light_vec);
    float3 light_direction_normal_ws = light_vec / d;

    float attenuation = DoAttenuation(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, d);
    
    float spot_intensity = DoSpotCone(light.DirectionVS.xyz, light_direction_normal_ws, light.SpotAngle);

    result.Diffuse = light.Color * DoDiffuse(normal_ws, light_direction_normal_ws) * attenuation * spot_intensity;
    result.Specular = light.Color * float4(BlinnPhongSpec(light_direction_normal_ws, normal_ws, to_eye_ws, pbr), 0.0f) * attenuation * spot_intensity;
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoDirectionalLightVS(DirectionalLight light, float3 V, float3 P, float3 N, float specularPower) {
    LightResult result;

    float3 L = normalize(-light.DirectionVS.xyz);

    result.Diffuse = light.Color * DoDiffuse(N, L);
    result.Specular = light.Color * DoSpecular(V, N, L, specularPower);
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoDirectionalLightWS(DirectionalLight light, BlinnPhongSpecMaterial pbr, float3 normal_ws, float3 to_eye_ws) {
    LightResult result;

    float3 light_direction_normal_ws = normalize(-light.DirectionWS.xyz);

    result.Diffuse = light.Color * DoDiffuse(normal_ws, light_direction_normal_ws);
    result.Specular = light.Color * float4(BlinnPhongSpec(light_direction_normal_ws, normal_ws, to_eye_ws, pbr), 0.0f);
    result.Ambient = light.Color * float4(light.Ambient, 0.0f);

    return result;
}

LightResult DoLightingVS(float3 P, float3 N, float specularPower) {
    uint i;

    // Lighting is performed in view space.
    float3 V = normalize(-P);

    LightResult totalResult = (LightResult)0;

    // Iterate point lights.
    for (i = 0; i < gLightPropertiesCB.NumPointLights; ++i) {
        LightResult result = DoPointLightVS(PointLights[i], V, P, N, specularPower);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    // Iterate spot lights.
    for (i = 0; i < gLightPropertiesCB.NumSpotLights; ++i) {
        LightResult result = DoSpotLightVS(SpotLights[i], V, P, N, specularPower);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    // Iterate directinal lights
    for (i = 0; i < gLightPropertiesCB.NumDirectionalLights; ++i) {
        LightResult result = DoDirectionalLightVS(DirectionalLights[i], V, P, N, specularPower);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);
    totalResult.Ambient = saturate(totalResult.Ambient);

    return totalResult;
}

LightResult DoLightingWS(float3 position_ws, float3 normal_ws, float3 to_eye_ws, BlinnPhongSpecMaterial pbr) {
    uint i;

    LightResult totalResult = (LightResult)0;

    // Iterate point lights.
    for (i = 0; i < gLightPropertiesCB.NumPointLights; ++i) {
        LightResult result = DoPointLightWS(PointLights[i], pbr, position_ws, normal_ws, to_eye_ws);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    // Iterate spot lights.
    for (i = 0; i < gLightPropertiesCB.NumSpotLights; ++i) {
        LightResult result = DoSpotLightWS(SpotLights[i], pbr, position_ws, normal_ws, to_eye_ws);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    // Iterate directinal lights
    for (i = 0; i < gLightPropertiesCB.NumDirectionalLights; ++i) {
        LightResult result = DoDirectionalLightWS(DirectionalLights[i], pbr, normal_ws, to_eye_ws);

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
        totalResult.Ambient += result.Ambient;
    }

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);
    totalResult.Ambient = saturate(totalResult.Ambient);

    return totalResult;
}


float3 ExpandNormal(float3 n) {
	return n * 2.0f - 1.0f;
}

float3 DoNormalMapping(float3x3 tbn, float3 normal_map_sample) {
	float3 normal_t = ExpandNormal(normal_map_sample);

    // Transform normal from tangent space to view space.
	float3 bumped_normal = mul(normal_t, tbn);
	return normalize(bumped_normal);
}

float3 DoNormalMapping(float3x3 tbn, Texture2D tex, float2 uv, bool need_inv_y) {
	float3 normal_map_sample = tex.Sample(TextureSampler, uv).xyz;
    //float3 normal_map_sample = LinearToSRGB(tex.Sample(TextureSampler, uv).xyz);
	float3 normal_t = ExpandNormal(normal_map_sample);
    if(need_inv_y) normal_t *= -1.0f;

    // Transform normal from tangent space to view space.
	float3 bumped_normal = mul(normal_t, tbn);
	return normalize(bumped_normal);
}

float3 DoBumpMapping(float3x3 tbn, Texture2D tex, float2 uv, float bump_scale) {
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
	normal = mul(normal, tbn);

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

float4 main(PixelShaderInput ps_in) : SV_Target {
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
    
	Material material = gMaterialData;
    
    float2 texture_uv = ps_in.TextureUV.xy;

    // By default, use the alpha component of the diffuse color.
	float alpha = material.Diffuse.a;
	if (material.HasTexture & HAS_OPACITY_TEXTURE) {
		alpha = OpacityTexture.Sample(TextureSampler, texture_uv).r;
	}

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
        //diffuse_albedo.xyz = SRGBToLinear(diffuse_albedo.xyz);
        //diffuse_albedo.xyz = LinearToSRGB(diffuse_albedo.xyz);
	}
    
	float specular_power = material.SpecularPower;
	if (material.HasTexture & HAS_SPECULAR_POWER_TEXTURE) {
		specular_power *= 1.0f - SpecularPowerTexture.Sample(TextureSampler, texture_uv).r; // aiTextureType_SHININESS->roughness
	}
    
    float k = (material.IndexOfRefraction - 1.0f) / (material.IndexOfRefraction + 1.0f);
    float k2 = k * k;
    float3 fresnelR0 = max(k2, material.Reflectance).xyz;
    if (material.HasTexture & HAS_METALNESS_TEXTURE) {
        float3 metalness_sample = MetalnessTexture.Sample(TextureSampler, texture_uv).rgb;
		fresnelR0 = lerp(fresnelR0, diffuse_albedo.rgb, metalness_sample);
	}

	float3 normal_t_ws;
    //normal_t_ws = normalize(ps_in.NormalWS);
    // Normal mapping
	if (material.HasTexture & HAS_NORMAL_TEXTURE) {
        float3 normal_ws = normalize(ps_in.NormalWS);
        //float3 tangent_ws = normalize(ps_in.TangentWS - dot(ps_in.TangentWS, normal_ws) * normal_ws);
		float3 tangent_ws = normalize(ps_in.TangentWS);
		float3 bitangent_ws = normalize(ps_in.BitangentWS);
        //float3 bitangent_ws = cross(normal_ws, tangent_ws);
    
		float3x3 tbn_ws = float3x3(tangent_ws, bitangent_ws, normal_ws);
    
		normal_t_ws = DoNormalMapping(tbn_ws, NormalTexture, texture_uv, material.HasTexture & HAS_NORMAL_INV_Y_TEXTURE);
	}
	else if (material.HasTexture & HAS_BUMP_TEXTURE) {
		float3 tangent_ws = normalize(ps_in.TangentWS);
		float3 bitangent_ws = normalize(ps_in.BitangentWS);
        float3 normal_ws = normalize(ps_in.NormalWS);
    
		float3x3 tbn_ws = float3x3(tangent_ws, -bitangent_ws, normal_ws);
    
		normal_t_ws = DoBumpMapping(tbn_ws, BumpTexture, texture_uv, material.BumpIntensity);
	}
	else {
		normal_t_ws = normalize(ps_in.NormalWS);
	}

	float shadow = 1.0f;
	if (material.HasTexture & HAS_SHADOW_TEXTURE) {
        shadow = CalcShadowFactor(ps_in.ShadowPosHS);
	}
    
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //float3 eye_position_w = gPerPassData.InverseTransposeViewMatrix._14_24_34;
    float3 eye_position_w = gPerPassData.InverseTransposeViewMatrix._41_42_43;
    float3 to_eye = eye_position_w - ps_in.PositionWS.xyz;
    float distance_to_eye = length(to_eye);
    float3 to_eye_normal_ws = normalize(to_eye);
    
	BlinnPhongSpecMaterial pbr = (BlinnPhongSpecMaterial)0.0f;
    pbr.FresnelR0 = fresnelR0;
    pbr.Shininess = specular_power;

    LightResult lit = DoLightingWS(ps_in.PositionWS.xyz, normal_t_ws, to_eye_normal_ws, pbr);
    ambient *= diffuse_albedo * lit.Ambient;
    diffuse_albedo *= lit.Diffuse;
    // Specular power less than 1 doesn't really make sense.
    // Ignore specular on materials with a specular power less than 1.
    if (material.SpecularPower > 1.0f) {
        specular = material.Specular;
        if (material.HasTexture & HAS_SPECULAR_TEXTURE) {
            specular = SampleTexture( SpecularTexture, texture_uv, specular );
        }
        specular *= lit.Specular;
    }
    
    float4 result = float4(ambient.rgb + (emissive + diffuse_albedo + specular).rgb * shadow, alpha * material.Opacity);
    //float fog_start = 1.0f;
    //float fog_range = 3.0f;
    //float3 fog_color = float3(0.729412f, 0.72549f, 0.705882f) * 0.5f;
    //float fog_lerp = saturate((distance_to_eye - fog_start) / fog_range);
    //result = float4(lerp(result.xyz, fog_color, fog_lerp), result.w);
    
    float fog_start = FogPropertiesCB.FogStart;
    float fog_range = FogPropertiesCB.FogRange;
    float3 fog_color = FogPropertiesCB.FogColor;
    float fog_lerp = saturate((distance_to_eye - fog_start) / fog_range);
    result = float4(lerp(result.xyz, fog_color, fog_lerp), result.w);
    
    return float4(LinearToSRGB(result.xyz), result.a);
	//return result;
}