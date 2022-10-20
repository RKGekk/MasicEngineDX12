struct PerPassData {
	float4x4 ViewMatrix;
	float4x4 InverseTransposeViewMatrix;
	
	float4x4 ProjectionMatrix;
	float4x4 InverseTransposeProjectionMatrix;
	
	float4x4 ViewProjectionMatrix;
	float4x4 InverseTransposeViewProjectionMatrix;
	
	float2 RenderTargetSize;
    float2 InverseRenderTargetSize;
	
    float  NearZ;
    float  FarZ;
    float  TotalTime;
    float  DeltaTime;
	
	float4x4 ShadowTransform;
};

ConstantBuffer<PerPassData> gPerPassData : register(b0);

struct InstanceData {
	float4x4 World;
	float4x4 InverseTransposeWorld;
	float4x4 TexureUVTransform;
};

StructuredBuffer<InstanceData> gInstanceData : register(t0, space1);

struct InstanceIndexData {
	uint InstanceIndex;
	uint Pad1;
	uint Pad2;
	uint Pad3;
};

StructuredBuffer<InstanceIndexData> gInstanceIndexData : register(t1, space1);

struct VertexPositionNormalTangentBitangentTexture {
	float3 PositionLS  : POSITION;
	float3 NormalLS    : NORMAL;
	float3 TangentLS   : TANGENT;
	float3 BitangentLS : BITANGENT;
	float3 TextureUV   : TEXCOORD;
};

struct VertexShaderOutput {
	float4 PositionHS  : SV_Position;
	float4 PositionWS  : POSITION;
	float3 NormalWS    : NORMAL;
	float3 TangentWS   : TANGENT;
	float3 BitangentWS : BITANGENT;
	float2 TextureUV   : TEXCOORD;
	float4 ShadowPosHS : SHADOW;
};

VertexShaderOutput main(VertexPositionNormalTangentBitangentTexture vs_in, uint instanceID : SV_InstanceID) {
	VertexShaderOutput vout = (VertexShaderOutput)0.0f;
	
	uint instance_index = gInstanceIndexData[instanceID].InstanceIndex;
	InstanceData instance_data = gInstanceData[instance_index];
	float4x4 world = instance_data.World;
	float4x4 world_inv_t = instance_data.InverseTransposeWorld;
	float4x4 texture_transform = instance_data.TexureUVTransform;
	
	//float4 wp = world._41_42_43_44;
	//float4 pos_ws = mul(float4(vs_in.PositionLS, 1.0f), world) + wp;
	float4 pos_ws = mul(float4(vs_in.PositionLS, 1.0f), world);
    vout.PositionWS = pos_ws;
	
	float4 pos_hs = mul(gPerPassData.ViewProjectionMatrix, pos_ws);
	vout.PositionHS = pos_hs;
	
	float3 normal_ws = mul(vs_in.NormalLS, (float3x3)world_inv_t);
	vout.NormalWS = normal_ws;
	
	float3 tangent_ws = mul(vs_in.TangentLS, (float3x3)world_inv_t);
	//float3 tangent_ws = -1.0f * mul(vs_in.TangentLS, (float3x3)world_inv_t);
	vout.TangentWS = tangent_ws;
	
	//float3 bitangent_ws = cross(normal_ws, tangent_ws);
	float3 bitangent_ws = mul(vs_in.BitangentLS, (float3x3)world_inv_t);
	vout.BitangentWS = bitangent_ws;
	
	float4 texure_uv = mul(float4(vs_in.TextureUV, 1.0f), texture_transform);
	vout.TextureUV = texure_uv.xy;
	
	float4 shadow_pos_hs = mul(pos_ws, gPerPassData.ShadowTransform);
	vout.ShadowPosHS = shadow_pos_hs;
	
	return vout;
}