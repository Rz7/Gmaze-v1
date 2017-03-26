// Ôàéë cube_texture.fx
//-----------------------------------------------------------------------
// Ñòðóêòóðû äàííûõ
//-----------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float3 Normal : TEXTCOORD0;
};


matrix World;
matrix View;
matrix Projection;

float4 Color;
float4 LightColor;
float4 LightDirection;


PS_INPUT VS( VS_INPUT Data )
{
    PS_INPUT Out;
    Out = (PS_INPUT)0;
    

    Out.Pos = mul (Data.Pos, World);
    Out.Pos = mul (Out.Pos, View);
    Out.Pos = mul (Out.Pos, Projection);

    Out.Tex = Data.Tex;

	Out.Normal = mul (Data.Normal, World);
	
    return Out;
}

float4 PS_Texture( PS_INPUT input ) : SV_Target
{
	float4 LCFinal = saturate(0.3+LightColor*dot((float3)LightDirection, input.Normal));
	float4 TextColor = Color;
	if(LCFinal.r <= 0.25f)
		LCFinal.r = 0.25f;
		
	if(LCFinal.g <= 0.25f)
		LCFinal.g = 0.25f;
		
	if(LCFinal.b <= 0.25f)
		LCFinal.b = 0.25f;
	
	float4 FinalColor = TextColor * LCFinal;

	FinalColor.a = 1.0f;
    return FinalColor;
}

technique10 RenderTexture
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Texture() ) );
    }
}