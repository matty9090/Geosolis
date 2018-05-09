/*float4x4 mWorld;
float3 mAmbient;
float3 mLightPos;
float4 mLightColour;*/

struct VS_OUTPUT {
	float4 Position    : SV_POSITION;
	float3 WorldPos    : NORMAL1;
	float3 Normal	   : NORMAL2;
};

static const float PI = 3.14159265f;

sampler2D tex0;
	
float4 main(VS_OUTPUT v) : SV_Target {
	return float4(0.0f, 0.4f, 1.0f, 0.32f);
}