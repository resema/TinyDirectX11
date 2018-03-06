//
// globals
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

//
// typedefs
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

//
// pixel shader
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	// sample the pixel color from the texture using the sampler 
	//  at this texture coord location
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// invert the light direction for calculations
	lightDir = -lightDirection;

	// calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));	// saturate clamps values within range of 0 to 1

	// determine the final amount of diffuse color based on the 
	//  diffuse color combined with the light intensiy
	color = saturate(diffuseColor * lightIntensity);

	// multiply the texture pixel and the final diffuse color 
	//  to get the final pixel color result
	color = color * textureColor;

	return color;
}