struct PerPassData {
	matrix ViewMatrix;
	matrix InverseViewMatrix;
	matrix InverseTransposeViewMatrix;
	
	matrix ProjectionMatrix;
	matrix InverseProjectionMatrix;
	matrix InverseTransposeProjectionMatrix;
	
	matrix ViewProjectionMatrix;
	matrix InverseViewProjectionMatrix;
	matrix InverseTransposeViewProjectionMatrix;
	
	float2 RenderTargetSize;
    float2 InverseRenderTargetSize;
    float  NearZ;
    float  FarZ;
    float  TotalTime;
    float  DeltaTime;
};

ConstantBuffer<PerPassData> gPerPassData : register(b0);

struct InstanceData {
	matrix World;
	matrix InverseTransposeWorld;
	matrix TexureUVTransform;
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
	float3 PositionLS : POSITION;
	float3 NormalLS   : NORMAL;
	float3 TangentLS  : TANGENT;
	float2 TextureUV  : TEXCOORD;
};

struct VertexShaderOutput {
	float4 PositionHS  : SV_Position;
	float3 PositionWS  : POSITION;
	float3 NormalWS    : NORMAL;
	float3 TangentWS   : TANGENT;
	float3 BitangentWS : BITANGENT;
	float2 TextureUV   : TEXCOORD;
};

VertexShaderOutput main(VertexPositionNormalTangentBitangentTexture vs_in, uint instanceID : SV_InstanceID) {
	VertexShaderOutput vout = (VertexShaderOutput)0.0f;
	
	uint instance_index = gInstanceIndexData[instanceID].InstanceIndex;
	InstanceData instance_data = gInstanceData[instance_index];
	float4x4 world = instance_data.World;
	float4x4 world_inv_t = instance_data.InverseTransposeWorld;
	float4x4 texture_transform = instance_data.TexureUVTransform;
	
	float4 pos_ws = mul(float4(vs_in.PositionLS, 1.0f), world);
    vout.PositionWS = pos_ws.xyz;
	
	float4 pos_hs = mul(gPerPassData.ViewProjectionMatrix, pos_ws);
	vout.PositionHS = pos_hs;
	
	float3 normal_ws = mul(vs_in.NormalLS, (float3x3)world_inv_t);
	vout.NormalWS = normal_ws;
	
	float3 tangent_ws = mul(vs_in.TangentLS, (float3x3)world_inv_t);
	vout.TangentWS = tangent_ws;
	
	float3 bitangent_ws = cross(normal_ws, tangent_ws);
	vout.BitangentWS = bitangent_ws;
	
	float4 texure_uv = mul(float4(vs_in.TextureUV, 0.0f, 1.0f), texture_transform);
	vout.TextureUV = texure_uv.xy;
	
	return vout;
}