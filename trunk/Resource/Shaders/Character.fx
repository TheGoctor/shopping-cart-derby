// globals
float4x4	gWorld;
float4x4	gWVP;
float3      gLightDirection;
float3      gViewPosition;
texture     Tex;
texture		Tex2;
texture		Tex3;
bool		gbFlashingYellow = false;
bool		gbFlashingWhite  = false;
bool		gbFlashingBlue   = false;
float		gFlashRate;
	
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

OutputVS OutlineVS(float3 position:POSITION0, float3 normal:NORMAL0, float2 uv0:TEXCOORD0)
{
	// output structure
	OutputVS outVS;
	
	outVS.posH = mul(float4(position, 1.0f), gWVP);	
	
	//outVS.normal = float4(normal, 1.0);
	
	outVS.color.a = dot(normal, gViewPosition);
	outVS.color.r = 0.0;
	outVS.color.g = 0.0;
	outVS.color.b = 0.0;
	
	outVS.texCoord0 = uv0;
	outVS.texCoord1.x = dot(normal, gViewPosition);
	outVS.texCoord1.y = 0.0;
	if(outVS.texCoord1.x < 0.0f)
		outVS.texCoord1 *= -1.0f;
	
    return outVS;
}

OutputVS DiffuseVS(float3 posLocal : POSITION0, float3 normalL : NORMAL0, float2 uv0:TEXCOORD0)
{
    OutputVS Out = (OutputVS)0;
    Out.posH = mul(float4(posLocal, 1.0f), gWVP); // transform Position

	// calculate the world position of the vertex    
    float4 worldPosition = mul(float4(posLocal, 1.0f), gWorld);
    
    // normalize the light vector
    //gLightDirection = normalize(gLightDirection);
    
    // multiply the normal by the world matrix to put the normal into the same
    // space as the light
    float3 normalW = mul(float4(normalL, 1.0f), gWorld).xyz;
	normalW = normalize(normalW);
    
    // calculate how much light is hitting this vertex
	float lightAmount = saturate(dot(gLightDirection, normalW));
	
	// amount of light * (material color * light color)
	Out.color.rgb = (diffuseMtrlColor*diffuseLightColor).rgb * lightAmount;
	
	//Out.color.rgb = normalW;
	
	// set the alpha to 1.0
	Out.color.a   = 1.0;
	
	// pass through the texture coordinates
	Out.texCoord0 = uv0;
	Out.texCoord1 = normalL.xy;

    return Out;
}

// Pixel Shader ---------------------------------------------------------------------------
float4 DiffusePS(float4 color : COLOR0, float2 uv0:TEXCOORD0) : COLOR
{
	//return float4(uv0, 1, 1);
	
	float4 ambient = gAmbientMtrl * gAmbientLight;
	
	float4 texCol = tex2D(samp1, uv0);
	
	if(texCol.a < 0.6)
		discard;
	
	return  (color + ambient) * texCol;
}

float4 AlphaPS(float4 color : COLOR0, float2 uv0:TEXCOORD0) : COLOR
{
	//return float4(uv0, 1, 1);
	
	float4 ambient = gAmbientMtrl * gAmbientLight;
	
	float4 texCol = tex2D(samp1, uv0);
	
	if(texCol.a <= 0.6)
		discard;
		
	// Flash Yellow
	if(gbFlashingYellow)
	{
		texCol.x *= 3 * gFlashRate;
		texCol.y *= 3 * gFlashRate;
	}
	
	if(gbFlashingWhite)
	{
		texCol.x = 1.0;
		texCol.y = 1.0;
		texCol.z = 1.0;
	}
	
	if(gbFlashingBlue)
	{
		texCol.z = 1.0;
	}
	
	return  texCol;
}

// Pixel Shader ---------------------------------------------------------------------------
float4 BlackPS(float4 color : COLOR0, float2 uv0:TEXCOORD0, float2 uv1:TEXCOORD1) : COLOR
{
	float4 texCol = tex2D(samp1, uv0);
	
	if(texCol.a <= 0.6)
		discard;
	
	float4 outCol = texCol;
	
	if(uv1.x <= 0.1)
		outCol = float4(0.0, 0.0, 0.0, 1.0);
		
	return outCol;
}

// ----------------------------------------------------------------------------------------
technique Animated
{
	pass P0
    {
        vertexShader = compile vs_2_0 TransformVS();
        pixelShader  = compile ps_2_0 AlphaPS();
        
        //AlphaTestEnable = true;
        //AlphaRef = 1.0f;
        //AlphaFunc = Greater;
        
        SrcBlend = 2;
		DestBlend = 1;
      
		ShadeMode = Flat;
        FillMode = Solid;
        CullMode = NONE;
        
        ZWriteEnable = true;
        ZEnable = true;
    }
}

technique Outline
{
    pass P0
    {
        vertexShader = compile vs_2_0 OutlineVS();
        pixelShader  = compile ps_2_0 BlackPS();
        
        //AlphaTestEnable = true;
        //AlphaRef = 1.0f;
        //AlphaFunc = Greater;
        
        SrcBlend = 2;
		DestBlend = 1;
      
		ShadeMode = Flat;
        FillMode = Solid;
        CullMode = NONE;
        
        ZWriteEnable = true;
        ZEnable = true;
    }
}