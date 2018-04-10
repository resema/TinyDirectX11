//
// globals
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float specularPower;
	float4 specularColor;
};

//
// typedefs
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

//
// pixel shader
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float3 reflection;
	float4 specular;

	// sample the pixel color from the texture using the sampler 
	//  at this texture coord location
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// set the default output color to the ambient light value for all pixels
	color = ambientColor;

	// initialize the specular color
	specular = float4(0.f, 0.f, 0.f, 0.f);

	// invert the light direction for calculations
	lightDir = -lightDirection;

	// calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));	// saturate clamps values within range of 0 to 1
	if (lightIntensity > 0.f)
	{
		// determine the final amount of diffuse color based on the 
		//  diffuse color combined with the light intensiy
		color += (diffuseColor * lightIntensity);

		// saturate the ambient and diffuse color
		color = saturate(color);

		// calculate the reflection vector based on the light intensity, normal vector and light direction
		//  walk twice the lightIntensity in the direction of input.normal and subtract
		//  the light direction l to get the reflection vector r
		reflection = normalize(2 * lightIntensity * input.normal - lightDir);

		// determine the amount of specular light based on the 
		//  reflection vector, viewing direction and specular power
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	}

	// Multiply the texture pixel and the input color to get the textured result.
	color = color * textureColor;

	// Add the specular component last to the output color.
	color = saturate(color + specular);


	return color;
}