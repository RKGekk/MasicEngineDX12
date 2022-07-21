cbuffer cbPerFrame : register(b0) {
	float4 gEyePosW;
	float4 gAtlasDim;
};

cbuffer cbPerObject : register(b1) {
	float4x4 gViewProj;
};

static const float2 gTexC[4] = {
	float2(0.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 1.0f),
	float2(1.0f, 0.0f)
};


struct VertexOut {
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
	float TTL : TTL;
};

struct GeoOut {
	float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 Tex : TEXCOORD;
	float TTL : TTL;
    //uint PrimID : SV_PrimitiveID;
};

[maxvertexcount(4)]
void main(point VertexOut gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream) {	
	float3 look = normalize(gEyePosW.xyz - gin[0].CenterW);
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), look));
	float3 up = cross(look, right);

	// Compute triangle strip vertices (quad) in world space.
	float halfWidth  = 0.5f * gin[0].SizeW.x;
	float halfHeight = 0.5f * gin[0].SizeW.y;
	
	float4 v[4];
	v[0] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);

	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	GeoOut gout;
	[unroll]
	for(int i = 0; i < 4; ++i) {
		gout.PosH = mul(v[i], gViewProj);
		gout.PosW = v[i].xyz;
		gout.NormalW = look;
		//gout.Tex = gTexC[i];
		//gout.Tex = float2(gTexC[i].x * gAtlasDim.x, gTexC[i].y * gAtlasDim.y);
		
		//float chunks_x = 1.0f / gAtlasDim.x;
		//float chunk_x = fmod((1.0f - gin[0].TTL), gAtlasDim.x);
		
		//float inv_ttl = (1.0f - gin[0].TTL);
		
		//float frame_raw_1_16 = inv_ttl / gAtlasDim.x * gAtlasDim.y;
		//float frame_1_16 = floor(frame_raw_1_16);
		
		//float frame_raw_1_4 = inv_ttl / gAtlasDim.y;
		//float frame_1_4 = floor(frame_raw_1_4);
		
		//float frame_norm_1_4 = frame_1_4 * gAtlasDim.y;
		//float frame_norm_1_16 = fmod(frame_1_16, 1.0f/gAtlasDim.x) * gAtlasDim.x;
		
		//gout.Tex = float2(frame_norm_1_4 + gTexC[i].x * gAtlasDim.x, gTexC[i].y * gAtlasDim.y);
		
		float inv_ttl = max(1.0f - gin[0].TTL - 0.01f, 0.0f);
		
		float frame_raw_1_16 = inv_ttl / (gAtlasDim.x * gAtlasDim.y);
		float frame_1_16 = floor(frame_raw_1_16);
		
		float frame_raw_1_4 = inv_ttl / gAtlasDim.y;
		float frame_1_4 = floor(frame_raw_1_4);
		
		float frame_norm_1_4 = clamp(frame_1_4 * gAtlasDim.y, 0.0f, 1.0f - gAtlasDim.y);
		float frame_norm_1_16 = clamp(fmod(frame_1_16, 1.0f/gAtlasDim.x) * gAtlasDim.x, 0.0f, 1.0f - gAtlasDim.x);
		
		gout.Tex = float2(gTexC[i].x * gAtlasDim.x, frame_norm_1_4 + gTexC[i].y * gAtlasDim.y);
		
		//gout.PrimID = primID;
		//gout.TTL = 1.0f;
		gout.TTL = gin[0].TTL;
		
		triStream.Append(gout);
	}
}