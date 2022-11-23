struct Matrices {
	float4x4 ModelMatrix;
	float4x4 ModelViewMatrix;
	float4x4 InverseTransposeModelViewMatrix;
	float4x4 ModelViewProjectionMatrix;
	float4x4 ShadowTransformMatrix;
};

ConstantBuffer<Matrices> MatricesCB : register(b0);

struct SkinnedData {
	float4x4 BoneTransforms[96];
	float4x4 InverseTransposeBoneTransforms[96];
};

ConstantBuffer<SkinnedData> gBoneTransforms : register(b3);

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
	float4 PositionVS  : POSITION;
	float3 NormalVS    : NORMAL;
	float3 TangentVS   : TANGENT;
	float3 BitangentVS : BITANGENT;
	float2 TexCoordTS  : TEXCOORD;
	float4 PositionHS  : SV_Position;
	float4 ShadowPosHS : SHADOW;
	//float4 PositionWS  : POSITION2;
};

VertexShaderOutput main(VertexPositionNormalTangentBitangentTexture IN) {
	VertexShaderOutput OUT;
	
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = IN.BoneWeights.x;
    weights[1] = IN.BoneWeights.y;
    weights[2] = IN.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 pos_accum_ls = float3(0.0f, 0.0f, 0.0f);
    float3 normal_accum_ls = float3(0.0f, 0.0f, 0.0f);
    float3 tangent_accum_ls = float3(0.0f, 0.0f, 0.0f);
	float3 bitangent_accum_ls = float3(0.0f, 0.0f, 0.0f);
	
	float3 pos_ls = IN.PositionLS;
    float3 normal_ls = IN.NormalLS;
    float3 tangent_ls = IN.TangentLS;
	float3 bitangent_ls = IN.BitangentLS;
	
    for(int i = 0; i < 4; ++i) {
		uint bone_idx = IN.BoneIndices[i];
		
        pos_accum_ls += weights[i] * mul(float4(pos_ls, 1.0f), gBoneTransforms.BoneTransforms[bone_idx]).xyz;
        normal_accum_ls += weights[i] * mul(normal_ls, (float3x3)gBoneTransforms.InverseTransposeBoneTransforms[bone_idx]);
        tangent_accum_ls += weights[i] * mul(tangent_ls, (float3x3)gBoneTransforms.InverseTransposeBoneTransforms[bone_idx]);
		bitangent_accum_ls += weights[i] * mul(bitangent_ls, (float3x3)gBoneTransforms.InverseTransposeBoneTransforms[bone_idx]);
    }

	pos_ls = pos_accum_ls;
    normal_ls = normal_accum_ls;
    tangent_ls = tangent_accum_ls;
	bitangent_ls = bitangent_accum_ls;

	OUT.PositionVS = mul(MatricesCB.ModelViewMatrix, float4(pos_ls, 1.0f));
	OUT.NormalVS = mul((float3x3) MatricesCB.InverseTransposeModelViewMatrix, normal_ls);
	OUT.TangentVS = mul((float3x3) MatricesCB.InverseTransposeModelViewMatrix, tangent_ls);
	OUT.BitangentVS = mul((float3x3) MatricesCB.InverseTransposeModelViewMatrix, bitangent_ls);
	OUT.TexCoordTS = IN.TextureUV.xy;
	OUT.PositionHS = mul(MatricesCB.ModelViewProjectionMatrix, float4(pos_ls, 1.0f));
	//OUT.ShadowPosHS = mul(MatricesCB.ShadowTransformMatrix, mul(float4(pos_ls, 1.0f), MatricesCB.ModelMatrix));
	OUT.ShadowPosHS = mul(mul(MatricesCB.ModelMatrix, float4(pos_ls, 1.0f)), MatricesCB.ShadowTransformMatrix);
	//OUT.PositionWS = mul(MatricesCB.ModelMatrix, float4(pos_ls, 1.0f));

	return OUT;
}