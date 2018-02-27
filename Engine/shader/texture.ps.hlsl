//
// globals
Texture2D shaderTexture;
SamplerState SampleType;

//
// Typedefs
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//
// Pixel Shader
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	
	// sample the pixel color from the texture using the sampler at this texture coord location
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	return textureColor;
}