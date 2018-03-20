#include "fontshaderclass.h"

FontShaderClass::FontShaderClass()
	: m_vertexShader(nullptr), m_pixelShader(nullptr), m_layout(nullptr),
	m_constantBuffer(nullptr), m_sampleState(nullptr), m_pixelBuffer(nullptr)
{
}

FontShaderClass::FontShaderClass(const FontShaderClass& other)
{
}

FontShaderClass::~FontShaderClass()
{
}

bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// initialize the vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"./shader/font.vs.hlsl", L"./shader/font.ps.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void FontShaderClass::Shutdown()
{
	// shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}

bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMVECTOR pixelColor)
{
	bool result;

	// set the shader parameters that it will use for rendering
	result = SetShaderParameters(
		deviceContext,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		texture,
		pixelColor
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
}

bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;

	// initialize the pointers
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// compile the vertex shader code
	result = D3DCompileFromFile(
		vsFilename,							// filename
		NULL,								// ptr to array of macros
		NULL,								// ptr to an include interface
		"FontVertexShader",				// name of the shader function
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
		"FontPixelShader", 				// name of the shader function
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

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

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

	// setup the description of the dynamic constant buffer that is in the vertex shader
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// create the constant buffer pointer to be accessed in the vertex shader
	result = device->CreateBuffer(
		&constantBufferDesc,
		NULL,
		&m_constantBuffer
		);
	if (FAILED(result))
	{
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

	// setup the description of the dynamic pixel constant buffer that is in the PS
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// create the pixel constant buffer pointer to be accessed in pixel shader
	result = device->CreateBuffer(
		&pixelBufferDesc,
		NULL,
		&m_pixelBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void FontShaderClass::ShutdownShader()
{
	// release the pixel constant buffer
	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = nullptr;
	}

	// release the sampler state
	if (m_sampleState) {
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	// release the matrix constant buffer
	if (m_constantBuffer) {
		m_constantBuffer->Release();
		m_constantBuffer = nullptr;
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

void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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
bool FontShaderClass::SetShaderParameters(
	ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMVECTOR pixelColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBufferType* dataPtr;
	unsigned int bufferNumber;
	PixelBufferType* dataPtr2;

	// lock the constant buffer so it can be written to
	result = deviceContext->Map(
		m_constantBuffer,
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
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	// transpose the matrices to prepare them for the shader
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// copy the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// unlock the constant buffer
	deviceContext->Unmap(
		m_constantBuffer,
		0
		);

	// set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	// finally set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(
		bufferNumber,
		1,
		&m_constantBuffer
		);

	// set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(
		0,			// start Slot
		1,			// num views
		&texture	// shader resource view
		);

	// lock the PIXEL constant buffer so it can be written to
	result = deviceContext->Map(
		m_pixelBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
		);
	if (FAILED(result))
	{
		return false;
	}

	// get a ptr to the data in the PIXEL constant buffer
	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	// copy the pixel color into the pixel constant buffer
	dataPtr2->pixelColor = pixelColor;

	// unlock the pixel constant buffer
	deviceContext->Unmap(
		m_pixelBuffer,
		0
		);

	// set the position of the pixel constant in the pixel shader
	bufferNumber = 0;

	// now set the pixel constant buffer in the pixel shader with the updated value
	deviceContext->PSSetConstantBuffers(
		bufferNumber,
		1,
		&m_pixelBuffer
		);

	return true;
}

void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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