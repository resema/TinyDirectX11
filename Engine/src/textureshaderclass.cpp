#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// initialize the vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"./shader/texture.vs.hlsl", L"./shader/texture.ps.hlsl");
	if (!result) {
		return false;
	}
	return true;
}

void TextureShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}

bool TextureShaderClass::Render(
	ID3D11DeviceContext* deviceContext, 
	int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	bool result;

	// set the shader parameters that it will use for rendering
	result = SetShaderParameters(
		deviceContext, 
		worldMatrix, viewMatrix, projectionMatrix,
		texture);
	if (!result) {
		return false;
	}

	// now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;		// contains the compiled vs shader
	ID3D10Blob* pixelShaderBuffer;		// contains the compiled ps shader
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];	// vertex and color
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;		// description of the texture sampler

	// initialize the pointers
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// compile the VERTEX SHADER code
	result = D3DCompileFromFile(
		vsFilename,							// filename
		NULL,								// ptr to array of macros
		NULL,								// ptr to an include interface
		"TextureVertexShader",				// name of the shader function
		"vs_5_0",							// version of the shader
		D3D10_SHADER_ENABLE_STRICTNESS,		// compile flags
		0,									// effect flags
		&vertexShaderBuffer,				// compiled shader
		&errorMessage						// lists of errors and warnings
		);
	if (FAILED(result)) {
		// if the shader failed to compile it has written something into the error message
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// if there is nothing in the error message then simply could not find the shader itself
		else {
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// compile the PIXEL SHADER code
	result = D3DCompileFromFile(
		psFilename,							// filename
		NULL, 								// ptr to array of macros
		NULL, 								// ptr to an include interface
		"TexturePixelShader", 				// name of the shader function
		"ps_5_0",							// version of the shader
		D3D10_SHADER_ENABLE_STRICTNESS, 	// compile flags
		0, 									// effect flags
		&pixelShaderBuffer, 				// compiled shader
		&errorMessage						// lists of errors and warnings
		);
	if (FAILED(result)) {
		// if the shader failed to compile it has written something into error message
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// if there is nothing then simply could not find the file itself
		else {
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// create the vertex shader from the buffer
	result = device->CreateVertexShader(
		vertexShaderBuffer->GetBufferPointer(),	// ptr to the start of the shader buffer
		vertexShaderBuffer->GetBufferSize(),	// size of buffer
		NULL,									// ptr to a class linkage interface
		&m_vertexShader							// returned vertex shader
		);
	if (FAILED(result)) {
		return false;
	}

	// create the pixel shader from the buffer
	result = device->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),	// ptr to the start of the shader buffer
		pixelShaderBuffer->GetBufferSize(), 	// size of buffer
		NULL, 									// ptr to a class linkage interface
		&m_pixelShader							// returned pixel shader
		);
	if (FAILED(result)) {
		return false;
	}

	// create the vertex input layout description
	//  this setup needs to match the VertexType structure in the ModelClass and Shader
	polygonLayout[0].SemanticName = "POSITION";		// must match shader variable name
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	//polygonLayout[1].SemanticName = "COLOR";		// must match shader variable name
	//polygonLayout[1].SemanticIndex = 0;
	//polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//polygonLayout[1].InputSlot = 0;
	//polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// get a count of elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// create the vertex input layout
	result = device->CreateInputLayout(
		polygonLayout,							// vertex layout
		numElements,							// nbr of elements in vertex layout
		vertexShaderBuffer->GetBufferPointer(), // ptr to the start of vertex shader
		vertexShaderBuffer->GetBufferSize(),	// size of vertex shader
		&m_layout								// returned ptr to input layout
		);
	if (FAILED(result)) {
		return false;
	}

	// release the vertex shader buffer and pixel shader buffer since not needed any longer
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// setup the description of the dynamic matrix CONSTANT BUFFER that is in the VS
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// create the constant buffer pointer so we can access the vertex shader constant
	//  buffer from within the class
	result = device->CreateBuffer(
		&matrixBufferDesc,
		NULL,
		&m_matrixBuffer
		);
	if (FAILED(result)) {
		return false;
	}

	// create a texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// filter method
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// behavior if U is larger than 1 or less than 0
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// behavior if V is larger than 1 or less than 0
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// behavior if W is larger than 1 or less than 0
	samplerDesc.MipLODBias = 0.f;							// offset from calculated mipmap level
	samplerDesc.MaxAnisotropy = 1;							// filter	
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// compare mipmap data with another mipmaps sampled data for this texture
	samplerDesc.BorderColor[0] = 0;							
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;									// lowest mipmap level (0=most detailed and largest)
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;					// largest mipmap level (0=most detailed and largest)

	// create the texture sampler state
	result = device->CreateSamplerState(
		&samplerDesc,
		&m_sampleState
		);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	// release the sampler state
	if (m_sampleState) { 
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	// release the matrix constant buffer
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	// release the layout
	if (m_layout) {
		m_layout->Release();
		m_layout = nullptr;
	}

	// release the pixel shader
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	// release the vertex shader
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	return;
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;
	std::ofstream fout;

	// get a pointer to the error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// get the length of the message
	bufferSize = errorMessage->GetBufferSize();

	// open a file to write the error message to
	fout.open("shader-error.txt");

	// write out the error message
	for (unsigned long long i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	// close the file 
	fout.close();

	// release the error message
	errorMessage->Release();
	errorMessage = nullptr;

	// pop a message up on the screen to notify the user to check the file
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

// send matrices into the vertex shader during Render function call
bool TextureShaderClass::SetShaderParameters(
	ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// transpose the matrices to prepare them for the shader
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// lock the constant buffer so it can be written to
	result = deviceContext->Map(
		m_matrixBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
		);
	if (FAILED(result)) {
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

	// finally set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(
		bufferNumber,
		1,
		&m_matrixBuffer
		);

	// set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(
		0,			// start Slot
		1,			// num views
		&texture	// shader resource view
		);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	// set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(
		m_vertexShader,			// vertex shader
		NULL,					// only used if shader uses interfaces
		0						// number of class-instances in the array
		);
	deviceContext->PSSetShader(
		m_pixelShader,			// pixel shader
		NULL, 					// only used if shader uses interfaces
		0						// number of class-instances in the array
		);

	// set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(
		0,				// start slot
		1,				// number of samplers
		&m_sampleState	// sampler
		);

	// render the triangles
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}