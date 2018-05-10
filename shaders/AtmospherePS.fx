struct VS_OUTPUT {
	float4 Position : POSITION;
	float3 Colour	: COLOR0;
	float  Alpha	: COLOR1;
};
	
float4 main(VS_OUTPUT v) : SV_Target {
	return float4(v.Colour, v.Alpha);
}