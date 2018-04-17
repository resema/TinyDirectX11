#include "multitextureshaderclass.h"

MultiTextureShaderClass::MultiTextureShaderClass()
	: m_vertexShader(nullptr), m_pixelShader(nullptr), m_layout(nullptr),
	m_matrixBuffer(nullptr), m_samplerState(nullptr)
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
	result = SetShaderParameters(
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
	if (FAILED(result))
	{
		return false;
	}

	// CREATE the PIXEL shader from the buffer
	result = device->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		NULL,
		&m_pixelShader
	);
	if (FAILED(result))
	{
		return false;
	}

	// create the vertex input layout description
	//  this needs to match the VertexType struct in the ModelClass and in the Shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// get a count of the elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// create the vertex input layout
	result = device->CreateInputLayout(
		polygonLayout,
		numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&m_layout
	);
	if (FAILED(result))
	{ 
		return false;
	}

	// release the vertex shader buffer and pixel shader buffer since they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// setup the description of the matrix dynamic constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// create the matrix constant buffer pointer to access the shader cbuffer
	result = device->CreateBuffer(
		&matrixBufferDesc,
		NULL,
		&m_matrixBuffer
	);
	if (FAILED(result))
	{
		return false;
	}

	// crete a texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// create the texture sampler state
	result = device->CreateSamplerState(
		&samplerDesc,
		&m_samplerState
	);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void MultiTextureShaderClass::ShutdownShader()
{
	// release the sampler state 
	if (m_samplerState)
	{
		m_samplerState->Release();
		m_samplerState = nullptr;
	}

	// release the matrix constant buffer
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	// release the layout
	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	// release the pixels shader
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	// release the vertex shader
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	return;
}

void MultiTextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage,
	HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize;
	std::ofstream fout;

	// get a pointer to the error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// get the length of the message
	bufferSize = errorMessage->GetBufferSize();

	// open a file to write the error message to
	fout.open("multitextureshader-error.txt");

	// write out the error message
	for (int i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// close the file
	fout.close();

	// release the error message
	errorMessage->Release();
	errorMessage = nullptr;

	// pop a message up on the screen to notify the user to check the text file for compile errors
	MessageBox(hwnd, L"Error compiling shader. Check multitextureshader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool MultiTextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView** textureArray)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// transpose the matrices to prepare them for the shader
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// LOCK the matrix constant buffer so it can be written to
	result = deviceContext->Map(
		m_matrixBuffer,				// resource
		0,							// sub-resource
		D3D11_MAP_WRITE_DISCARD,	// map type
		0,							// map flags
		&mappedResource				// mapped resource
	);
	if (FAILED(result))
	{
		return false;
	}

	// GET A POINTER to the data in the CONSTANT BUFFER
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// COPY the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// UNLOCK the matrix constant buffer
	deviceContext->Unmap(
		m_matrixBuffer,		// resource
		0					// sub-resource
	);

	// set the POSITION OF THE MATRIX CONSTANT BUFFER in the vertex shader
	bufferNumber = 0;

	// now SET THE MATRIX CONSTANT BUFFER in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(
		bufferNumber,		// start slot
		1,					// number of buffers
		&m_matrixBuffer		// constant buffer
	);

	// SET SHADER TEXTURE ARRAY RESOURCE in the pixel shader
	deviceContext->PSSetShaderResources(
		0,				// start slot
		2,				// number of textures in the array
		textureArray	// texture resource
	);

	return true;
}

void MultiTextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	// set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	// render the triangles
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}