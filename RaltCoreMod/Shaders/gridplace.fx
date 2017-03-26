// Ôàéë cube_texture.fx
//-----------------------------------------------------------------------
// Ñòðóêòóðû äàííûõ
//-----------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

float4 Color;
matrix World;
matrix View;
matrix Projection;

PS_INPUT VS( VS_INPUT Data )
{
    PS_INPUT Out;
    Out = (PS_INPUT)0;

    Out.Pos = mul (Data.Pos, World);
    Out.Pos = mul (Out.Pos, View);
    Out.Pos = mul (Out.Pos, Projection);
    return Out;
}

float4 PS_Texture( PS_INPUT input ) : SV_Target
{
    return Color;
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