float4x4 mWorldViewProj;
float4x4 mInvWorld;
float4x4 mTransWorld;
float3 mLightPos;
float4 mLightColor;

struct VS_OUTPUT
{
	float4 Position   : POSITION;
	float4 Diffuse    : COLOR0;
	float2 TexCoord   : TEXCOORD0;
};

struct VS_INPUT {
	float4 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT v_in)
{
	VS_OUTPUT Output;

	Output.Position = mul(v_in.vPosition, mWorldViewProj);
	float3 normal = mul(v_in.vNormal, mInvWorld);
	normal = normalize(normal);
	
	float3 worldpos = mul(mTransWorld, v_in.vPosition);
	float3 lightVector = worldpos - mLightPos;
	lightVector = normalize(lightVector);
	
	float3 tmp = dot(-lightVector, normal);
	tmp = lit(tmp.x, tmp.y, 1.0);
	
	tmp = mLightColor * tmp.y;
	Output.Diffuse = float4(tmp.x, tmp.y, tmp.z, 0);
	Output.TexCoord = v_in.texCoord;
	
	return Output;
}