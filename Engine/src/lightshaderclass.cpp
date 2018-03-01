#include "lightshaderclass.h"

LightShaderClass::LightShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_samplerState = nullptr;
	m_matrixBuffer = nullptr;
	m_lightBuffer = nullptr;
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// initialize the vertex and pixel shaders
	result = InitializeShader(device, hwnd, 
		L"./shader/light.vs.hlsl", L"./shader/light.ps.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void LightShaderClass::Shutdown() {
	// shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}