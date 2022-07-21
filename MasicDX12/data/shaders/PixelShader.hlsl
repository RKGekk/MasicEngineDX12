struct PixelShaderInput {
	float4 Color : COLOR;
	float4 Position : SV_Position;
};

float4 main(PixelShaderInput IN) : SV_Target {
	return IN.Color;
}