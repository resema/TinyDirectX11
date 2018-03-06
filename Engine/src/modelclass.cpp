#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Texture = nullptr;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	bool result;

	// initialize the vertex and index buffer
	result = InitializeBuffers(device);
	if (!result) 
	{
		return false;
	}

	// load the texture for this model
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// release the model texture
	ReleaseTexture();

	// shutdown the vertex and index buffers
	ShutdownBuffers();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc,indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// set the number of vertices in the vertex array
	m_vertexCount = 4;

	// set the number of indices in the index array
	m_indexCount = 6;

	// create the vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// create the index array
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// load the vertex array data
	vertices[0].position = XMFLOAT3(-1.f, -1.f, 0.f);	// bottom left
	vertices[0].texture = XMFLOAT2(0.f, 1.f);
	vertices[0].normal = XMFLOAT3(0.f, 0.f, -1.f);
	
	vertices[1].position = XMFLOAT3(-1.f, 1.f, 0.f);	// top left
	vertices[1].texture = XMFLOAT2(0.f, 0.f);
	vertices[1].normal = XMFLOAT3(0.f, 0.f, -1.f);

	vertices[2].position = XMFLOAT3(1.f, 1.f, 0.f);		// top right 
	vertices[2].texture = XMFLOAT2(1.f, 0.f);
	vertices[2].normal = XMFLOAT3(0.f, 0.f, -1.f);

	vertices[3].position = XMFLOAT3(1.f, -1.f, 0.f);	// bottom right
	vertices[3].texture = XMFLOAT2(1.f, 1.f);
	vertices[3].normal = XMFLOAT3(0.f, 0.f, -1.f);

	// load the index array with data
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	// set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// give the subresource structure a pointer to the VERTEX DATA
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// now create the vertex buffer
	result = device->CreateBuffer(
		&vertexBufferDesc,		// ptr to buffer description
		&vertexData,			// ptr to subresource data structure
		&m_vertexBuffer			// returned ID3D11 buffer
		);
	if (FAILED(result)) {
		return false;
	}

	// set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// give the subresource structure a pointer to the INDEX DATA
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create the index buffer
	result = device->CreateBuffer(
		&indexBufferDesc,		// ptr to buffer description
		&indexData, 			// ptr to subresource data structure
		&m_indexBuffer			// returned ID3D11 buffer
		);
	if (FAILED(result)) {
		return false;
	}

	// release the array that the vertex and index buffers have been created and loaded
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// release the index buffer
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// release the vertex buffer
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	// set vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(
		0,					// input slot to bind to
		1,					// number of buffers to bind
		&m_vertexBuffer,	// ptr to actual vertex buffer
		&stride,			// size of each vertex
		&offset				// offset in bytes from the beginning
		);

	// set index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(
		m_indexBuffer, 
		DXGI_FORMAT_R32_UINT, 
		0
		);

	// set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		//D3D11_PRIMITIVE_TOPOLOGY_LINELIST
		//D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
		);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// create the texture object
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// initialize the texture object
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// release the texture object
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}
	
	return;
}
