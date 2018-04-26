#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_TextureArray = nullptr;
	m_Model = nullptr;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2, 
	WCHAR* textureFilename3, WCHAR* textureFilename4)
{
	bool result;

	// load in the model data
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// calculate the normal, tangent and binormal for the model
	CalculateModelVectors();

	// initialize the vertex and index buffer
	result = InitializeBuffers(device);
	if (!result) 
	{
		return false;
	}

	// load the texture for this model
	result = LoadTextures(
		device, 
		textureFilename1, 
		textureFilename2, 
		textureFilename3, 
		textureFilename4
	);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// release the model texture
	ReleaseTextures();

	// shutdown the vertex and index buffers
	ShutdownBuffers();

	// release the model data
	ReleaseModel();

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

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc,indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

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

	// load the vertex array and index array with data
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_Model[i].x, m_Model[i].y, m_Model[i].z);
		vertices[i].texture = XMFLOAT2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].normal = XMFLOAT3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);
		vertices[i].tangent = XMFLOAT3(m_Model[i].tx, m_Model[i].ty, m_Model[i].tz);
		vertices[i].binormal = XMFLOAT3(m_Model[i].bx, m_Model[i].by, m_Model[i].bz);

		indices[i] = i;
	}

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

bool ModelClass::LoadTextures(ID3D11Device* device, 
	WCHAR* filename1, WCHAR* filename2, WCHAR* filename3, WCHAR* filename4)
{
	bool result;

	// create the texture array object
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray)
	{
		return false;
	}

	// initialize the texture array object
	result = m_TextureArray->Initialize(device, filename1, filename2, filename3, filename4);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTextures()
{
	// release the texture object
	if (m_TextureArray)
	{
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = nullptr;
	}
	
	return;
}

bool ModelClass::LoadModel(char* filename)
{
	std::ifstream fin;
	char input;

	// open the model file
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// read up the value of vertex coutn
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// read in the vertex count
	fin >> m_vertexCount;

	// set the number of indics to be the same as the vertex count
	m_indexCount = m_vertexCount;

	// create the model using the vertex count that was read in
	m_Model = new ModelType[m_vertexCount];
	if (!m_Model)
	{
		return false;
	}

	// read up to the beginning of the data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);		// first newline
	fin.get(input);		// second newline

	// read in the vertex data
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_Model[i].x >> m_Model[i].y >> m_Model[i].z;
		fin >> m_Model[i].tu >> m_Model[i].tv;
		fin >> m_Model[i].nx >> m_Model[i].ny >> m_Model[i].nz;
	}

	// close the model file
	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_Model)
	{
		delete[] m_Model;
		m_Model = nullptr;
	}

	return;
}

void ModelClass::CalculateModelVectors()
{
	int faceCount;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;

	// calculate the number of faces in the model
	faceCount = m_vertexCount / 3;

	// initialize the index to the model data
	int idx = 0;

	// go through all the faces and calculate the tangent, binormal and normal vectors
	for (int i = 0; i < faceCount; i++)
	{
		// get the three vertices for this face from the model
		vertex1.x = m_Model[idx].x;
		vertex1.y = m_Model[idx].y;
		vertex1.z = m_Model[idx].z;
		vertex1.tu = m_Model[idx].tu;
		vertex1.tv = m_Model[idx].tv;
		vertex1.nx = m_Model[idx].nx;
		vertex1.ny = m_Model[idx].ny;
		vertex1.nz = m_Model[idx].nz;
		idx++;

		vertex2.x = m_Model[idx].x;
		vertex2.y = m_Model[idx].y;
		vertex2.z = m_Model[idx].z;
		vertex2.tu = m_Model[idx].tu;
		vertex2.tv = m_Model[idx].tv;
		vertex2.nx = m_Model[idx].nx;
		vertex2.ny = m_Model[idx].ny;
		vertex2.nz = m_Model[idx].nz;
		idx++;

		vertex3.x = m_Model[idx].x;
		vertex3.y = m_Model[idx].y;
		vertex3.z = m_Model[idx].z;
		vertex3.tu = m_Model[idx].tu;
		vertex3.tv = m_Model[idx].tv;
		vertex3.nx = m_Model[idx].nx;
		vertex3.ny = m_Model[idx].ny;
		vertex3.nz = m_Model[idx].nz;
		idx++;

		// calculate the tangent and binormal of that face
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// calculate the new normal using the tangent and binormal
		CalculateNormal(tangent, binormal, normal);

		// store the normal, tangent and binormal for this face back in the model structure
		m_Model[idx - 1].nx = normal.x;
		m_Model[idx - 1].ny = normal.y;
		m_Model[idx - 1].nz = normal.z;
		m_Model[idx - 1].tx = tangent.x;
		m_Model[idx - 1].ty = tangent.y;
		m_Model[idx - 1].tz = tangent.z;
		m_Model[idx - 1].bx = binormal.x;
		m_Model[idx - 1].by = binormal.y;
		m_Model[idx - 1].bz = binormal.z;

		m_Model[idx - 2].nx = normal.x;
		m_Model[idx - 2].ny = normal.y;
		m_Model[idx - 2].nz = normal.z;
		m_Model[idx - 2].tx = tangent.x;
		m_Model[idx - 2].ty = tangent.y;
		m_Model[idx - 2].tz = tangent.z;
		m_Model[idx - 2].bx = binormal.x;
		m_Model[idx - 2].by = binormal.y;
		m_Model[idx - 2].bz = binormal.z;

		m_Model[idx - 3].nx = normal.x;
		m_Model[idx - 3].ny = normal.y;
		m_Model[idx - 3].nz = normal.z;
		m_Model[idx - 3].tx = tangent.x;
		m_Model[idx - 3].ty = tangent.y;
		m_Model[idx - 3].tz = tangent.z;
		m_Model[idx - 3].bx = binormal.x;
		m_Model[idx - 3].by = binormal.y;
		m_Model[idx - 3].bz = binormal.z;
	}

	return;
}

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2,
	TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den, length;

	// calculate the two vectors for this face
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// calculate the tu and tv texture space vectors
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// calculate the denominator of the tangent/binormal equation
	den = 1.f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// calculate the cross product and multiply by the coeff to get the tangent/binormal
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.x = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.x = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.x = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.x = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// calculate the length of this normal
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// normalize the normal and store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// calculate the length of this normal
	length = sqrt((binormal.x  * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// normalize the normal and store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void ModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;

	// calculate the cross product of the tangent and binormal which will give the normal vector
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// calculate the length of the normal
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// normalize the normal
	normal.x = normal.x / length;
	normal.y = normal.y / lenght;
	normal.z = normal.z / length;

	return;
}