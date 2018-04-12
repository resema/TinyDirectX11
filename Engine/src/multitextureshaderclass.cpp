#include "multitextureshaderclass.h"

MultiTextureShaderClass::MultiTextureShaderClass()
	: m_vertexShader(nullptr), m_pixelShader(nullptr), m_layout(nullptr),
	m_matrixBuffer(nullptr), m_sampleState(nullptr)
{
}

bool MultiTextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	
	// initialize the vertex
	result = InitializeShader(
		device,
		hwnd,
		L"../Engine/multitexture.vs.hlsl",
		L"../Engine/multitexture.ps.hlsl"
	);
	if (!result)
	{
		return false;
	}

	return true;
}

void MultiTextureShaderClass::Shutdown()
{
	// shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}

bool MultiTextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView** textureArray)
{
	bool result;

}