#ifndef LIGHTSHADERCLASS_H
#define LIGHTSHADERCLASS_H

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fstream>

class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMVECTOR diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;		// added extra padding so structure is a multiple of 16 for CreateBuffer function requirements
	};
};

#endif	// LIGHTSHADERCLASS_H