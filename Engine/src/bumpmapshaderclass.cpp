#include "bumpmapshaderclass.h"

BumpMapShaderClass::BumpMapShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_samplerState = nullptr;
	m_matrixBuffer = nullptr;
	m_cameraBuffer = nullptr;
	m_lightBuffer = nullptr;
}

BumpMapShaderClass::BumpMapShaderClass(const BumpMapShaderClass& other)
{
}

BumpMapShaderClass::~BumpMapShaderClass()
{
}

bool BumpMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// initialize the vertex and pixel shaders
	result = InitializeShader(
		device, 
		hwnd, 
		L"./shader/bumpmap.vs.hlsl", 
		L"./shader/specmap.ps.hlsl"
	);
	if (!result)
	{
		return false;
	}

	return true;
}

void BumpMapShaderClass::Shutdown()
{
	// shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}

bool BumpMapShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView** textureArray, 
	XMFLOAT3 lightDirection, XMVECTOR ambientColor, XMVECTOR diffuseColor,
	XMFLOAT3 cameraPosition, XMVECTOR specularColor, float specularPower)
{
	bool result;

	// set the shader parameters that it will use for rendering
	result = SetShaderParameters(
		deviceContext,
		worldMatrix, viewMatrix, projectionMatrix,
		textureArray,
		lightDirection, ambientColor, diffuseColor,
		cameraPosition, specularColor, specularPower
		);
	if (!result) 
	{
		return false;
	}

	// now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

bool BumpMapShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
	WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// initialize the pointers
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// compile the vertex shader code
	result = D3DCompileFromFile(
		vsFilename,							// filename
		NULL,								// ptr to array of macros
		NULL,								// ptr to an include interface
		"BumpMapVertexShader",				// name of the shader function
		"vs_5_0",							// version of the shader
		D3D10_SHADER_ENABLE_STRICTNESS,		// compile flags
		0,									// effect flags
		&vertexShaderBuffer,				// compiled shader
		&errorMessage						// lists of errors and warnings
		);
	if (FAILED(result))
	{
		// if the shader failed to compile it should have written somthing to error msg
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// if there was nothing in the error msg then it simply could not find the shader file
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	// compile the pixel shader code
	result = D3DCompileFromFile(
		psFilename,							// filename
		NULL,								// ptr to array of macros
		NULL,								// ptr to an include interface
		"BumpMapPixelShader",				// name of the shader function
		"ps_5_0",							// version of the shader
		D3D10_SHADER_ENABLE_STRICTNESS,		// compile flags
		0,									// effect flags
		&pixelShaderBuffer,					// compiled shader
		&errorMessage						// lists of errors and warnings
		);
	if (FAILED(result))
	{
		// if the shader failed to compile it should have written somthing to error msg
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// if there was nothing in the error msg then it simply could not find the shader file
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Pixel Shader File", MB_OK);
		}

		return false;
	}

	// create the vertex shader from the buffer
	result = device->CreateVertexShader(
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		NULL,
		&m_vertexShader
		);
	if (FAILED(result))
	{
		return false;
	}

	// create the pixel shader from the buffer
	result = device->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		NULL,
		&m_pixelShader
		);

	// create the vertex input layout description
	// this setup needs to match the vertextype structure in the ModelClass and shader
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

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

	// release the vertex shader buffer and pixel shader - not used anymore
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// create a texture sampler state description
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

	// setup the description of the dynamic matrix constant buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// create the ocnstant buffer ptr to access the vertex shader constant buffer
	result = device->CreateBuffer(
		&matrixBufferDesc,
		NULL,
		&m_matrixBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	// setup the desc of the camera dynamic constant buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// create the camera constant buffer pointer to access the vertex shader constant buffer
	result = device->CreateBuffer(
		&cameraBufferDesc,
		NULL,
		&m_cameraBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	// setup the desc of the light dynamic constant buffer
	//  note that Bytewidth always to be a multiple of 16
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// create the constant buffer ptr to acces the vertex shader constant buffer
	result = device->CreateBuffer(
		&lightBufferDesc,
		NULL,
		&m_lightBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void BumpMapShaderClass::ShutdownShader()
{
	// release the light constant buffer
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	// release the camera constant buffer
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = nullptr;
	}

	// release the matrix constant buffer
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	// release the sampler state
	if (m_samplerState)
	{
		m_samplerState->Release();
		m_samplerState = nullptr;
	}

	// release the layout
	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	// release the pixel shader
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

void BumpMapShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	size_t bufferSize;
	std::ofstream fout;

	// get ptr to error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// get lenght of the message
	bufferSize = errorMessage->GetBufferSize();

	// open a file to write the error message to
	fout.open("shader-error.txt");

	// write out the error message
	for (size_t i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// release the error message
	errorMessage->Release();
	errorMessage = nullptr;

	// pop a message up on the screen
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt.", shaderFilename, MB_OK);

	return;
}

bool BumpMapShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView** textureArray, 
	XMFLOAT3 lightDirection, XMVECTOR ambientColor, XMVECTOR diffuseColor,
	XMFLOAT3 cameraPosition, XMVECTOR specularColor, float specularPower)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	CameraBufferType* dataPtr3;

	// transpose the matrices to prepare them for the shader
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// lock the constant buffers so it can be written to
	result = deviceContext->Map(
		m_matrixBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
		);
	if (FAILED(result))
	{
		return false;
	}

	// get a pointer to the data in the constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// copy the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// unlock the constant buffer
	deviceContext->Unmap(
		m_matrixBuffer,
		0
		);

	// set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	// now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(
		bufferNumber,
		1,
		&m_matrixBuffer
		);

	// set shader texture resoure in pixel shader
	deviceContext->PSSetShaderResources(
		0,				// start slot
		5,				// number of textures in the array (two textures & one alpha map & one bumpmap) & one specmap
		textureArray	// texture resource array [5]
		);

	// lock the camera constant buffer so it can be written to
	result = deviceContext->Map(
		m_cameraBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
		);
	if (FAILED(result))
	{
		return false;
	}

	// get a pointer to the data in the constant buffer
	dataPtr3 = (CameraBufferType*)mappedResource.pData;

	// copy the camera position into the constant buffer
	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.f;

	// unlock the camera constant buffer
	deviceContext->Unmap(
		m_cameraBuffer, 
		0
		);

	// set the position of the camera constant buffer in the vertex shader
	bufferNumber = 1;

	// now set the camera constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(
		bufferNumber,
		1,
		&m_cameraBuffer
		);

	//
	// lock the light constant buffers so it can be written to
	result = deviceContext->Map(
		m_lightBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
		);
	if (FAILED(result))
	{
		return false;
	}

	// get a ptr to the data in the constant buffer
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// copy the light variables into the constant buffer
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;

	// unlock the constant buffer
	deviceContext->Unmap(
		m_lightBuffer,
		0
		);

	// set the position of the light constants buffer in the pixel shader
	bufferNumber = 0;

	// finally set the light constant buffer in the pixel shader with the updated values
	deviceContext->PSSetConstantBuffers(
		bufferNumber,
		1,
		&m_lightBuffer
		);

	return true;
}

void BumpMapShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	// set the vertex and pixel shaders
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(
		0,
		1,
		&m_samplerState
		);

	// render the triangle
	deviceContext->DrawIndexed(
		indexCount,
		0,
		0
		);
}