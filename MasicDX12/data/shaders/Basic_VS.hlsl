struct Matrices {
	float4x4 ModelMatrix;
	float4x4 ModelViewMatrix;
	float4x4 InverseTransposeModelViewMatrix;
	float4x4 ModelViewProjectionMatrix;
	float4x4 ShadowTransformMatrix;
};

ConstantBuffer<Matrices> MatricesCB : register(b0);

struct VertexPositionNormalTangentBitangentTexture {
	float3 Position  : POSITION;
	float3 Normal    : NORMAL;
	float3 Tangent   : TANGENT;
	float3 Bitangent : BITANGENT;
	float3 TexCoord  : TEXCOORD;
};

struct VertexShaderOutput {
	float4 PositionVS  : POSITION;
	float3 NormalVS    : NORMAL;
	float3 TangentVS   : TANGENT;
	float3 BitangentVS : BITANGENT;
	float2 TexCoordTS  : TEXCOORD;
	float4 PositionHS  : SV_Position;
	float4 ShadowPosHS : SHADOW;
};

VertexShaderOutput main(VertexPositionNormalTangentBitangentTexture IN) {
	VertexShaderOutput OUT;

	OUT.PositionVS  = mul(MatricesCB.ModelViewMatrix, float4(IN.Position, 1.0f));
	OUT.NormalVS    = mul((float3x3) MatricesCB.InverseTransposeModelViewMatrix, IN.Normal);
	OUT.TangentVS   = mul((float3x3) MatricesCB.InverseTransposeModelViewMatrix, IN.Tangent);
	OUT.BitangentVS = mul((float3x3) MatricesCB.InverseTransposeModelViewMatrix, IN.Bitangent);
	OUT.TexCoordTS  = IN.TexCoord.xy;
	OUT.PositionHS  = mul(MatricesCB.ModelViewProjectionMatrix, float4(IN.Position, 1.0f));
	OUT.ShadowPosHS = mul(MatricesCB.ShadowTransformMatrix, mul(float4(IN.Position, 1.0f), MatricesCB.ModelMatrix));

	return OUT;
}