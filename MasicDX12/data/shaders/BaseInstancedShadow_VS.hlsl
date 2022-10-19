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
};

VertexShaderOutput main(VertexPositionNormalTangentBitangentTexture vs_in, uint instanceID : SV_InstanceID) {
	VertexShaderOutput vout = (VertexShaderOutput)0.0f;
	
	uint instance_index = gInstanceIndexData[instanceID].InstanceIndex;
	InstanceData instance_data = gInstanceData[instance_index];
	float4x4 world = instance_data.World;
	
	float4 pos_ws = mul(float4(vs_in.PositionLS, 1.0f), world);
	float4 pos_hs = mul(gPerPassData.ViewProjectionMatrix, pos_ws);
	vout.PositionHS = pos_hs;
	
	return vout;
}