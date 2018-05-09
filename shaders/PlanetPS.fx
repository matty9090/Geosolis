float4x4 mWorld;
float3 mAmbient;
float3 mLightPos;
float4 mLightColour;

struct VS_OUTPUT {
	float4 Position    : SV_POSITION;
	float3 WorldPos    : NORMAL1;
	float3 Normal	   : NORMAL2;
};

static const float PI = 3.14159265f;

sampler2D tex0;
	
float4 main(VS_OUTPUT v) : SV_Target { 
	float2 tex;
	
	tex.x = 0.5f + (atan2(v.Normal.z, v.Normal.x) / (2.0f * PI));
	tex.y = 0.5f - (asin(v.Normal.y) / PI);

	float3 normal = normalize(mul(v.Normal, mWorld));
	float3 lightDir = normalize(mLightPos - v.WorldPos);
	float3 diffuse  = mLightColour * saturate(dot(normal, lightDir)) + mAmbient;
	
	return tex2D(tex0, tex) * float4(diffuse, 1.0f);
}