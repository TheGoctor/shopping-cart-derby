// globals
float4x4	gWorld;
float4x4	gWVP;
float3      gLightDirection;
texture     Tex;
texture		Tex2;
texture		Tex3;
	
float4 gAmbientMtrl;		// ambient material color
float4 gAmbientLight;		// ambient light color
							
float4 diffuseMtrlColor;	// the diffuse color of the material
float4 diffuseLightColor;	// the color of the diffuse light	

// texture sampler
sampler		samp1
{
	Texture = <Tex>;
	// texture filtering
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	//MaxAnisotropy = 4;
	// texture address modes
	AddressU  = WRAP;
	AddressV  = WRAP;
};

sampler		samp2
{
	Texture = <Tex2>;
	// texture filtering
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	// texture address modes (available options are WRAP, MIRROR, BORDER, and CLAMP)
	AddressU  = WRAP;
	AddressV  = WRAP;
};

sampler		samp3
{
	Texture = <Tex3>;
	// texture filtering
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	// texture address modes (available options are WRAP, MIRROR, BORDER, and CLAMP)
	AddressU  = WRAP;
	AddressV  = WRAP;
};

// Define a vertex shader output structure;
struct OutputVS
{
    float4 posH      : POSITION0;
    float4 color     : COLOR0;
    float2 texCoord0 : TEXCOORD0;
    float2 texCoord1 : TEXCOORD1;
};

// Vertex shader -------------------------------------------------------------------------------------
OutputVS TransformVS(float3 position:POSITION0, float3 normal:NORMAL0, float2 uv0:TEXCOORD0)
{
	// output structure
	OutputVS outVS;
	
	outVS.posH = mul(float4(position, 1.0f), gWVP);	
	
	//outVS.normal = normal;
	
	outVS.color = float4(1,1,1,1);
	
	outVS.texCoord0 = uv0;
	outVS.texCoord1 = position.xy;
	
    return outVS;
}

// Pixel Shader ---------------------------------------------------------------------------
float4 AlphaPS(float4 color : COLOR0, float2 uv0:TEXCOORD0) : COLOR
{
	//return float4(uv0, 1, 1);
	
	float4 ambient = gAmbientMtrl * gAmbientLight;
	
	float4 texCol = tex2D(samp1, uv0);
	
	if(texCol.a < 0.1)
		discard;
	
	return  texCol;
}

// ----------------------------------------------------------------------------------------
technique AlphaTest
{
    pass P0
    {
        vertexShader = compile vs_2_0 TransformVS();
        pixelShader  = compile ps_2_0 AlphaPS();
        
        //AlphaTestEnable = true;
        //AlphaRef = 1.0f;
        //AlphaFunc = Greater;
        
        AlphaBlendEnable = true;
		SrcBlend = 3;
		DestBlend = 6;
      
		ShadeMode = Flat;
        FillMode = Solid;
        CullMode = NONE;
        
        ZWriteEnable = true;
        ZEnable = true;
    }
}