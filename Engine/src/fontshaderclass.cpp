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
	result = InititalizeShader(device, hwnd, L"./shader/font.vs.hlsl", L"./shader/font.ps.hlsl");
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
	result = SetShaderParameter(
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


}