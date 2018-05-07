struct VS_OUTPUT {
	float4 Position   : POSITION;
	float4 Diffuse    : COLOR0;
	float3 Normal	  : NORMAL;
};

static const float PI = 3.14159265f;

sampler2D tex0;
	
float4 main(VS_OUTPUT v) : SV_Target { 
	float2 tex;
	
	tex.x = 0.5f + (atan2(v.Normal.z, v.Normal.x) / (2.0f * PI));
	tex.y = 0.5f - (asin(v.Normal.y) / PI);

	float4 col = tex2D(tex0, tex) * v.Diffuse;
	
	return col;
}