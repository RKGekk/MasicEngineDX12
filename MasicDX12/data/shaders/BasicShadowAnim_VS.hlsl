struct Matrices {
	matrix ModelMatrix;
	matrix ModelViewMatrix;
	matrix InverseTransposeModelViewMatrix;
	matrix ModelViewProjectionMatrix;
};

ConstantBuffer<Matrices> MatricesCB : register(b0);

struct SkinnedData {
	float4x4 BoneTransforms[96];
	float4x4 InverseTransposeBoneTransforms[96];
};

ConstantBuffer<SkinnedData> gBoneTransforms : register(b3);

struct VertexPositionNormalTangentBitangentTexture {
	float3 Position    : POSITION;
	float3 Normal      : NORMAL;
	float3 Tangent     : TANGENT;
	float3 Bitangent   : BITANGENT;
	float3 TexCoord    : TEXCOORD;
	float3 BoneWeights : WEIGHTS;
	uint4  BoneIndices : BONEINDICES;
};

struct VertexShaderOutput {
	float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPositionNormalTangentBitangentTexture IN) {
	VertexShaderOutput OUT;
	
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = IN.BoneWeights.x;
    weights[1] = IN.BoneWeights.y;
    weights[2] = IN.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 pos_accum_ls = float3(0.0f, 0.0f, 0.0f);
	float3 pos_ls = IN.Position;
    for(int i = 0; i < 4; ++i) {
		uint bone_idx = IN.BoneIndices[i];
        pos_accum_ls += weights[i] * mul(float4(pos_ls, 1.0f), gBoneTransforms.BoneTransforms[bone_idx]).xyz;
    }
	pos_ls = pos_accum_ls;

	OUT.Position = mul(MatricesCB.ModelViewProjectionMatrix, float4(pos_ls, 1.0f));

	return OUT;
}