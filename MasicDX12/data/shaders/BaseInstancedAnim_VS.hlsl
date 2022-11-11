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

struct SkinnedData {
	float4x4 BoneTransforms[96];
	float4x4 InverseTransposeBoneTransforms[96];
};

ConstantBuffer<SkinnedData> gBoneTransforms : register(b3);

struct InstanceIndexData {
	uint InstanceIndex;
	uint Pad1;
	uint Pad2;
	uint Pad3;
};

StructuredBuffer<InstanceIndexData> gInstanceIndexData : register(t1, space1);

struct VertexPositionNormalTangentBitangentTextureAnim {
	float3 PositionLS  : POSITION;
	float3 NormalLS    : NORMAL;
	float3 TangentLS   : TANGENT;
	float3 BitangentLS : BITANGENT;
	float3 TextureUV   : TEXCOORD;
	float3 BoneWeights : WEIGHTS;
	uint4  BoneIndices : BONEINDICES;
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

VertexShaderOutput main(VertexPositionNormalTangentBitangentTextureAnim vs_in, uint instanceID : SV_InstanceID) {
	VertexShaderOutput vout = (VertexShaderOutput)0.0f;
	
	uint instance_index = gInstanceIndexData[instanceID].InstanceIndex;
	InstanceData instance_data = gInstanceData[instance_index];
	float4x4 world = instance_data.World;
	float4x4 world_inv_t = instance_data.InverseTransposeWorld;
	float4x4 texture_transform = instance_data.TexureUVTransform;
	
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vs_in.BoneWeights.x;
    weights[1] = vs_in.BoneWeights.y;
    weights[2] = vs_in.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 pos_accum_ls = float3(0.0f, 0.0f, 0.0f);
    float3 normal_accum_ls = float3(0.0f, 0.0f, 0.0f);
    float3 tangent_accum_ls = float3(0.0f, 0.0f, 0.0f);
	float3 bitangent_accum_ls = float3(0.0f, 0.0f, 0.0f);
	
	float3 pos_ls = vs_in.PositionLS;
    float3 normal_ls = vs_in.NormalLS;
    float3 tangent_ls = vs_in.TangentLS;
	float3 bitangent_ls = vs_in.BitangentLS;
	
    for(int i = 0; i < 4; ++i) {
		uint bone_idx = vs_in.BoneIndices[i];
		
        pos_accum_ls += weights[i] * mul(float4(pos_ls, 1.0f), gBoneTransforms.BoneTransforms[bone_idx]).xyz;
        normal_accum_ls += weights[i] * mul(normal_ls, (float3x3)gBoneTransforms.InverseTransposeBoneTransforms[bone_idx]);
        tangent_accum_ls += weights[i] * mul(tangent_ls, (float3x3)gBoneTransforms.InverseTransposeBoneTransforms[bone_idx]);
		bitangent_accum_ls += weights[i] * mul(bitangent_ls, (float3x3)gBoneTransforms.InverseTransposeBoneTransforms[bone_idx]);
    }

	pos_ls = pos_accum_ls;
    normal_ls = normal_accum_ls;
    tangent_ls = tangent_accum_ls;
	bitangent_ls = bitangent_accum_ls;
	
	float4 pos_ws = mul(float4(pos_ls, 1.0f), world);
    vout.PositionWS = pos_ws;
	
	float4 pos_hs = mul(gPerPassData.ViewProjectionMatrix, pos_ws);
	vout.PositionHS = pos_hs;
	
	float3 normal_ws = mul(normal_ls, (float3x3)world_inv_t);
	vout.NormalWS = normal_ws;
	
	float3 tangent_ws = mul(tangent_ls, (float3x3)world_inv_t);
	//float3 tangent_ws = -1.0f * mul(vs_in.TangentLS, (float3x3)world_inv_t);
	vout.TangentWS = tangent_ws;
	
	//float3 bitangent_ws = cross(normal_ws, tangent_ws);
	float3 bitangent_ws = mul(bitangent_ls, (float3x3)world_inv_t);
	vout.BitangentWS = bitangent_ws;
	
	float4 texure_uv = mul(float4(vs_in.TextureUV, 1.0f), texture_transform);
	vout.TextureUV = texure_uv.xy;
	
	float4 shadow_pos_hs = mul(pos_ws, gPerPassData.ShadowTransform);
	vout.ShadowPosHS = shadow_pos_hs;
	
	return vout;
}