cbuffer perObjectBuffer : register(b0) {
	float4x4 worldMatrix;
};

struct VertexIn {
	float3 PosL : POSITION;
	float2 SizeW : SIZE;
	float TTL : TTL;
};

struct VertexOut {
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
	float TTL : TTL;
};

VertexOut main(VertexIn vin) {
	VertexOut vout;
	
	//vout.CenterW = vin.PosW;
	vout.CenterW = mul(float4(vin.PosL, 1.0f), worldMatrix);
	vout.SizeW = vin.SizeW;
	vout.TTL = vin.TTL;

	return vout;
}