struct VS_OUTPUT
{
	float4 Position   : POSITION;
	float4 Diffuse    : COLOR0;
	float2 TexCoord   : TEXCOORD0;
};

sampler2D tex0;
	
float4 main(VS_OUTPUT v) : SV_Target
{ 
	float4 col = tex2D(tex0, v.TexCoord) * v.Diffuse;
	
	return col;
}