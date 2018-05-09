float4x4 mWorldViewProj;
float4x4 mWorld;

struct VS_OUTPUT
{
	float4 Position    : SV_POSITION;
	float3 WorldPos    : NORMAL1;
	float3 Normal	   : NORMAL2;
};

struct VS_INPUT {
	float4 vPosition : POSITION;
	float3 vNormal   : NORMAL;
};

VS_OUTPUT main(VS_INPUT v_in) {
	VS_OUTPUT Output;

	Output.Position = mul(v_in.vPosition, mWorldViewProj);	
	Output.WorldPos = mul(v_in.vPosition, mWorld);
	Output.Normal	= v_in.vNormal;

	return Output;
}