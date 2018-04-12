Texture2D shaderTextures[2];
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	flaot2 tex : TEXCOORD0;
};

float4 MultiTexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 color1;
	float4 color2;
	float4 blendColor;

	// get the pixel color from the first texture
	color1 = shaderTextures[0].Sampe(SampleType, input.tex);

	// get the pixel color from the second texture
	color2 = shaderTextures[1].Sample(SampleType, input.text);

	// blend the two pixels together and multiply by the gamma value
	blendColor = color1 * color2 * 2.0;

	// saturate the final color
	blendColor = saturate(blendColor);

	return blendColor;
}