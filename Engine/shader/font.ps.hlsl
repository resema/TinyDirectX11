// globals
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer PixelBuffer
{
	float4 pixelColor;
};

// typedefs
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 FontPixelShader(PixelInputType input) : SV_TARGET
{
	float4 color;

	// sample the texture pixel at this location
	color = shaderTexture.Sample(SampleType, input.tex);

	// if the color is black on the texture then treat this pixel as transparent
	if (color.r == 0.f)
	{
		color.a = 0.f;
	}
	// if the color is other than black on the texture then this is a pixel in the font
	else
	{
		color.a = 1.f;
		color = color * pixelColor;
	}

	return color;
}