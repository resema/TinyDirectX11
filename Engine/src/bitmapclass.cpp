#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Texture = nullptr;
}

BitmapClass::BitmapClass(const BitmapClass& other)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight,
	char* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	// store the screen size
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// store the size in pixels that this bitmap should be rendered at
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// initialize the previous rendering position to negative one
	m_previousPosX = -1;
	m_previousPosY = -1;

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

void BitmapClass::Shutdown()
{
	// release the model texture
	ReleaseTexture();

	// shutdown the vertex and index buffers
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	// re-build the DYNAMIC VERTEX BUFFER for rendering to possibly a different location
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	// put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// set the number of vertices in the vertex array
	m_vertexCount = 6;

	// set the number of indices in the index array
	m_indexCount = m_vertexCount;

	// create the vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// create the index array
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// initialize vertex array to zeros at first
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// load the index array with data
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// set up the description of the DYNAMIC VERTEX BUFFER
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// now create the vertex buffer
	result = device->CreateBuffer(
		&vertexBufferDesc,
		&vertexData,
		&m_vertexBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	// set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// give the subresource structure a ptr to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create the index buffer
	result = device->CreateBuffer(
		&indexBufferDesc,
		&indexData,
		&m_indexBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	// release the arrays now that the vertex and index buffers
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	// release the index buffer
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// release the vertex buffer
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// if the position we are rendering this bitmap to has not changed then don't 
	//  upgrade the vertex buffer since it currently is correct
	if (positionX == m_previousPosX && positionY == m_previousPosY)
	{
		return true;
	}

	// if it has changed then update the position it is being renderend to
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// calculate the screen coordinates of the left, right, top and bottom of the bitmap
	left = (float)((m_screenWidth / 2) * (-1)) + (float)positionX;
	right = left + (float)m_bitmapWidth;
	
	top = (float)(m_screenHeight / 2) - (float)positionY;
	bottom = top - (float)m_bitmapHeight;

	// create a temporary vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// load the vertex array with data
	// first triangle
	int idx = 0;
	vertices[idx].position = XMFLOAT3(left, top, 0.f);		// top left
	vertices[idx++].texture = XMFLOAT2(0.f, 0.f);

	vertices[idx].position = XMFLOAT3(right, bottom, 0.f);	// bottom right
	vertices[idx++].texture = XMFLOAT2(1.f, 1.f);

	vertices[idx].position = XMFLOAT3(left, bottom, 0.f);	// bottom left
	vertices[idx++].texture = XMFLOAT2(0.f, 1.f);

	// second triangle
	vertices[idx].position = XMFLOAT3(left, top, 0.f);		// top left
	vertices[idx++].texture = XMFLOAT2(0.f, 0.f);

	vertices[idx].position = XMFLOAT3(right, top, 0.f);		// top right
	vertices[idx++].texture = XMFLOAT2(1.f, 0.f);

	vertices[idx].position = XMFLOAT3(right, bottom, 0.f);	// bottom right
	vertices[idx++].texture = XMFLOAT2(1.f, 1.f);

	// lock the vertex buffer so it can be written to
	result = deviceContext->Map(
		m_vertexBuffer,				// resource
		0,							// sub-resource
		D3D11_MAP_WRITE_DISCARD,	// map type
		0,							// mapflags
		&mappedResource				// mapped sub-resource
		);
	if (FAILED(result))
	{
		return false;
	}

	// get a pointer to the data in the vertex buffer
	verticesPtr = (VertexType*)mappedResource.pData;

	// copy the data into the vertex buffer
	memcpy(
		verticesPtr,							// destination
		(void*)vertices,						// source
		(sizeof(VertexType) * m_vertexCount)	// size
		);

	// unlock the vertex buffer
	deviceContext->Unmap(
		m_vertexBuffer,
		0
		);

	// release the vertex array 
	delete[] vertices;
	vertices = nullptr;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	// set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(
		0,					// start slot
		1,					// nbr of buffer
		&m_vertexBuffer,	// ptr of vertex buffer
		&stride,			// stride
		& offset			// offset
		);

	// set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(
		m_indexBuffer,			// index buffer
		DXGI_FORMAT_R32_UINT,	// format
		0						// offset
		);

	// set the type of primitive that should be rendered from this vertex buffer
	deviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST		// topology
		);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
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

void BitmapClass::ReleaseTexture()
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