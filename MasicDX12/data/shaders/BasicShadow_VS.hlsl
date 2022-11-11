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

	OUT.Position = mul(MatricesCB.ModelViewProjectionMatrix, float4(IN.Position, 1.0f));

	return OUT;
}