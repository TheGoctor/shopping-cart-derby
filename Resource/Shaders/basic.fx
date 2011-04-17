// globals
float4x4	gWVP;		// world * view * projection
float		time;

// Define a vertex shader output structure;
struct OutputVS
{
    float4 posW:POSITION0;
    float4 color:COLOR0;
};

// Vertex shader -------------------------------------------------------------------------------------
OutputVS TransformVS(float3 position:POSITION0, float4 col:COLOR0)
{
	// output structure
	OutputVS outVS;

	float4 temp = float4(position, 1.0f);
	
	outVS.posW = mul(temp, gWVP);
	outVS.color = col;
	
    return outVS;
}

// Pixel Shader ---------------------------------------------------------------------------
float4 TransformPS(float4 color:COLOR0):COLOR
{
	return color;
}

// ----------------------------------------------------------------------------------------
technique Basic
{
    pass P0
    {
        vertexShader = compile vs_3_0 TransformVS();
        pixelShader  = compile ps_3_0 TransformPS();

		//ShadeMode = Flat;
        FillMode = Wireframe;
        CullMode = None;
    }
}
technique Flat
{
    pass P0
    {
        vertexShader = compile vs_3_0 TransformVS();
        pixelShader  = compile ps_3_0 TransformPS();

		ShadeMode = Flat;
        //FillMode = Wireframe;
        CullMode = None;
    }
}
