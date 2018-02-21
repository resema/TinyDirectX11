#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	// initialize the vertex and index buffer
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}
	return true;
}

void ModelClass::Shutdown()
{
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

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc,indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// set the number of vertices in the vertex array
	m_vertexCount = 6;

	// set the number of indices in the index array
	m_indexCount = 6;

	// create the vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// create the index array
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// load the vertex array data
	vertices[0].position = XMFLOAT3(-1.f, -1.f, 0.f);
	vertices[0].color = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
	
	vertices[1].position = XMFLOAT3(-1.f, 1.f, 0.f);
	vertices[1].color = XMFLOAT4(0.f, 1.f, 0.f, 1.f);

	vertices[2].position = XMFLOAT3(1.f, 1.f, 0.f);
	vertices[2].color = XMFLOAT4(0.f, 0.f, 1.f, 1.f);

	vertices[3].position = XMFLOAT3(-1.f, -1.f, 0.f);
	vertices[3].color = XMFLOAT4(1.f, 0.f, 0.f, 1.f);

	vertices[4].position = XMFLOAT3(1.f, 1.f, 0.f);
	vertices[4].color = XMFLOAT4(0.f, 0.f, 1.f, 1.f);

	vertices[5].position = XMFLOAT3(1.f, -1.f, 0.f);
	vertices[5].color = XMFLOAT4(0.f, 1.f, 0.f, 1.f);

	// load the index array with data
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

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
		&vertexBufferDesc, 
		&vertexData, 
		&m_vertexBuffer
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
		&indexBufferDesc, 
		&indexData, 
		&m_indexBuffer
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
		0, 
		1, 
		&m_vertexBuffer, 
		&stride, 
		&offset
		);

	// set index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(
		m_indexBuffer, 
		DXGI_FORMAT_R32_UINT, 
		0
		);

	// set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}