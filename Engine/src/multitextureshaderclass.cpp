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

	// set the shader parameters that it will use for rendering
	result = SetShaderParameter(
		deviceContext,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		textureArray
	);
	if (!result)
	{
		return false;
	}

	// now render the prepared buffers with the shader
	RenderShader(
		deviceContext,
		indexCount
	);

	return true;
}

bool MultiTextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
	WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage{ nullptr };
	ID3D10Blob* vertexShaderBuffer{ nullptr };
	ID3D10Blob* pixelShaderBuffer{ nullptr };
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// COMPILE the VERTEX shader code
	result = D3DCompileFromFile(
		vsFilename,							// filename
		NULL,								// defines
		NULL,								// includes
		"MultiTextureVertexShader",			// entry point
		"vs_5_0",							// target version
		D3D10_SHADER_ENABLE_STRICTNESS,		// compile flags
		0,									// effect flags
		&vertexShaderBuffer,				// compiled shader
		&errorMessage						// lists of errors and warnings
	);
	if (FAILED(result))
	{
		//if the shader fialed to compile it should have written something to the error msg
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// if there was nothing in the error msg then not shader file could be found
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// COMPILE the PIXEL shader code
	result = D3DCompileFromFile(
		psFilename,							// filename
		NULL,								// defines
		NULL,								// includes
		"MultiTexturePixelShader",			// entry point
		"ps_5_0",							// target shader version
		D3D10_SHADER_ENABLE_STRICTNESS,		// compile flags
		0,									// effect flags
		&pixelShaderBuffer,					// compiled shader
		&errorMessage						// list of errors and warnings
	);
	if (FAILED(result))
	{
		// if the shader failed to compile
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// if there is nothing in the error message
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// CREATE the VERTEX shader from the buffer
	result = device->CreateVertexShader(
		vertexShaderBuffer->GetBufferPointer(),	// ptr to the start of the shader buffer
		vertexShaderBuffer->GetBufferSize(),	// size of buffer
		NULL,									// ptr to a class linkage interface
		&m_vertexShader							// returned vertex shader
	);
}