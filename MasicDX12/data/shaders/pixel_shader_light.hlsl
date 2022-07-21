struct DirectionalLight {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

struct Material {
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

void ComputeDirectionalLight(Material mat, DirectionalLight L, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec) {
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 lightVec = -L.Direction;
	ambient = mat.Ambient * L.Ambient;	
	float diffuseFactor = dot(lightVec, normal);
	[flatten]
	if(diffuseFactor > 0.0f) {
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
}

void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,	out float4 ambient, out float4 diffuse, out float4 spec) {
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 lightVec = L.Position - pos;
	float d = length(lightVec);
	if (d > L.Range) { return; }
	lightVec /= d; 
	ambient = mat.Ambient * L.Ambient;	
	float diffuseFactor = dot(lightVec, normal);
	[flatten]
	if(diffuseFactor > 0.0f) {
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
	float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));
	diffuse *= att;
	spec    *= att;
}

void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec) {
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 lightVec = L.Position - pos;
	float d = length(lightVec);
	if (d > L.Range) { return; }
	lightVec /= d; 
	ambient = mat.Ambient * L.Ambient;	
	float diffuseFactor = dot(lightVec, normal);
	[flatten]
	if(diffuseFactor > 0.0f) {
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
	float att = spot / dot(L.Att, float3(1.0f, d, d * d));
	ambient *= spot;
	diffuse *= att;
	spec *= att;
}

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW) {
	float3 normalT = 2.0f*normalMapSample - 1.0f;
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	float3 bumpedNormalW = mul(normalT, TBN);
	return bumpedNormalW;
}

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow, Texture2D shadowMap, float4 shadowPosH) {
	shadowPosH.xyz /= shadowPosH.w;
	
	float depth = shadowPosH.z;
	const float dx = SMAP_DX;
	float percentLit = 0.0f;
	const float2 offsets[9] = {
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for(int i = 0; i < 9; ++i) {
		percentLit += shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy + offsets[i], depth).r;
		//percentLit += shadowMap.SampleCmp(samShadow, shadowPosH.xy + offsets[i], depth).r;
		//percentLit += shadowMap.Sample(samShadow, shadowPosH.xy + offsets[i]).r;
	}

	return percentLit /= 9.0f;
}

cbuffer perObjectBuffer : register(b0) {
	float4x4 lwvpMatrix;
	float4x4 invWorldMatrix;
	float4x4 worldMatrix;
	float4x4 shadowTransform;
	Material gMaterial;
};

cbuffer cbPerFrame : register(b1) {
	DirectionalLight gDirLights[3];
	PointLight gPointLights[3];
	SpotLight gSpotLights[3];
	
	int gDirLightCount;
	int gPointLightCount;
	int gSpotLightCount;
	float  gFogStart;
	
	float3 gEyePosW;
	float  gFogRange;
	
	float4 gFogColor; 
};

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tg : TANGENT;
	float3 worldPos : WORLD_POSITION;
	float4 ShadowPosH : TEXCOORD1;
};

Texture2D gDiffuseMap : TEXTURE1 : register(t0);
SamplerState DiffuseMapSamplerState : SAMPLER1 : register(s0);

Texture2D gNormalMap : TEXTURE2 : register(t1);
SamplerState NormalMapSamplerState : SAMPLER2 : register(s1);

Texture2D gShadowMap : TEXTURE3 : register(t2);
//SamplerState ShadowMapSamplerState : SAMPLER3 : register(s2);
SamplerComparisonState ShadowMapSamplerState : SAMPLER3 : register(s2);

float4 main(PS_INPUT input) : SV_TARGET {
	input.pos.x /= input.pos.w;
	input.pos.y /= input.pos.w;
	input.pos.z /= input.pos.w;
	
	input.normal = normalize(input.normal);
	float3 toEye = gEyePosW - input.worldPos;
	float distToEye = length(toEye);
	toEye /= distToEye;
    float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	texColor = gDiffuseMap.Sample(DiffuseMapSamplerState, input.uv);

	float3 normalMapSample = gNormalMap.Sample(NormalMapSamplerState, input.uv).rgb;
	//float3 normalMapSample = gNormalMap.Sample(NormalMapSamplerState, input.uv).rgb + input.pos.xyw;
	//float4 normalMapSample = gNormalMap.Sample(NormalMapSamplerState, input.uv).rgba + input.pos.xyzw;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.xyz, input.normal, input.tg);
	
	float shadow = CalcShadowFactor(ShadowMapSamplerState, gShadowMap, input.ShadowPosH);

	float4 litColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if(gDirLightCount > 0) {
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
		float4 Am, Di, Sp;
		ComputeDirectionalLight(gMaterial, gDirLights[0], bumpedNormalW, toEye, Am, Di, Sp);
		ambient += Am; diffuse += shadow * Di; spec += shadow * Sp;
		
		[unroll]
		for(int i = 1; i < gDirLightCount; ++i) {
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], bumpedNormalW, toEye, A, D, S);
			//ComputeDirectionalLight(gMaterial, gDirLights[i], input.normal, toEye, A, D, S);
			ambient += A; diffuse += D; spec += S;
			
			//ambient += A; diffuse += (shadow + A) * D; spec += (shadow + A) * S;
		}

		litColor += texColor * (ambient + diffuse) + spec;
	}
	
	if(gPointLightCount > 0) {
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
		[unroll]
		for(int i = 0; i < gPointLightCount; ++i) {
			float4 A, D, S;
			ComputePointLight(gMaterial, gPointLights[i], input.worldPos, bumpedNormalW, toEye, A, D, S);
			ambient += A; diffuse += D; spec += S;
			//ambient += A; diffuse += (shadow + A) * D; spec += (shadow + A) * S;
		}

		litColor += texColor * (ambient + diffuse) + spec;
	}
	
	if(gSpotLightCount > 0) {
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
		[unroll]
		for(int i = 0; i < gSpotLightCount; ++i) {
			float4 A, D, S;
			ComputeSpotLight(gMaterial, gSpotLights[i], input.worldPos, bumpedNormalW, toEye, A, D, S);
			ambient += A; diffuse += D; spec += S;
			//ambient += A; diffuse += (shadow + A) * D; spec += (shadow + A) * S;
		}

		litColor += texColor * (ambient + diffuse) + spec;
	}
 
	float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 
	litColor = lerp(litColor, gFogColor, fogLerp);
	
	litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
	//return texColor;
}