// globals
cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// typedefs
struct VertexInputType
{
	float4 positition : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType FontVertexShader(VertexInputType input)
{
	PixelInputType output;

	// change the position vector to be 4 units
	input.position.w = 1.f;

	// calculate the position of the vertex against the world, view, and proj matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	return output;
}