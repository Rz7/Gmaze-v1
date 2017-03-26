// dInterface.fx //
cbuffer MatrixBuffer
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_INPUT_POS
{
    float4 Pos 	: POSITION;
	float2 Tex 	: TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos  : SV_POSITION;
	float2 Tex  : TEXCOORD0;
};

bool lTex;				// Если true, то загружается текстура. Иначе ColorTexture
float visible;
float4 ColorTexture;
Texture2D g_pTexture;
SamplerState colorSampler : register(s0);

PS_INPUT VS(VS_INPUT_POS Data)
{
    PS_INPUT Out;
    Out = (PS_INPUT)0;
	
    Out.Pos = mul(Data.Pos, World);
	Out.Pos = mul(Out.Pos, View);
    Out.Pos = mul(Out.Pos, Projection);
    Out.Tex = Data.Tex;
    return Out;
}

float4 PS_Texture(PS_INPUT input) : SV_Target
{
	float4 TextColor;

	if(lTex == true)
		TextColor = g_pTexture.Sample(colorSampler, input.Tex);
	else
		TextColor = ColorTexture;
		
	//TextColor.a = visible;

	TextColor.a = visible;
	clip(TextColor.a - visible);
	
	return TextColor;
}

technique10 RenderTexture
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_Texture()));
    }
}