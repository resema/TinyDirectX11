//
// globals
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//
// typedefs
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

//
// Vertex Shader
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;

	// change the position vector to be homogenous coord
	input.position.w = 1.f;

	// calculate the position of the vertex against world, view and proj matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	// calculate the normal vector against the world matrix only
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// normalize the normal vector
	output.normal = normalize(output.normal);

	return output;
}