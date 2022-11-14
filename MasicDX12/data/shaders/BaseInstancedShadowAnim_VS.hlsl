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

struct VertexPositionNormalTangentBitangentTexture {
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
};

VertexShaderOutput main(VertexPositionNormalTangentBitangentTexture vs_in, uint instanceID : SV_InstanceID) {
	VertexShaderOutput vout = (VertexShaderOutput)0.0f;
	
	uint instance_index = gInstanceIndexData[instanceID].InstanceIndex;
	InstanceData instance_data = gInstanceData[instance_index];
	float4x4 world = instance_data.World;
	
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vs_in.BoneWeights.x;
    weights[1] = vs_in.BoneWeights.y;
    weights[2] = vs_in.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
	
	float3 pos_accum_ls = float3(0.0f, 0.0f, 0.0f);
	float3 pos_ls = vs_in.PositionLS;
	
    for(int i = 0; i < 4; ++i) {
		uint bone_idx = vs_in.BoneIndices[i];
        pos_accum_ls += weights[i] * mul(float4(pos_ls, 1.0f), gBoneTransforms.BoneTransforms[bone_idx]).xyz;
    }

	pos_ls = pos_accum_ls;
	
	float4 pos_ws = mul(float4(pos_ls, 1.0f), world);
	float4 pos_hs = mul(gPerPassData.ViewProjectionMatrix, pos_ws);
	vout.PositionHS = pos_hs;
	
	return vout;
}