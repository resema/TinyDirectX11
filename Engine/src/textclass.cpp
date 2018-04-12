#include "textclass.h"

// pre-processing directives
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

TextClass::TextClass()
	: m_Font(nullptr), m_FontShader(nullptr),
	  m_sentence1(nullptr), m_sentence2(nullptr), m_sentence3(nullptr)
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
	HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	bool result;

	// store the screen width and height
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// store the base view matrix
	m_baseViewMatrix = baseViewMatrix;

	// create the font object
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// initialize the font object
	result = m_Font->Initialize(device, deviceContext, "./data/fontdata.txt", "./data/font_conv.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// create the font shader object
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// initialize the font shader object
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// initialize the first sentence
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	// now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_sentence1, "Hello World", 300, 50, 1.f, 1.f, 1.f, deviceContext);
	if (!result)
	{
		return false;
	}

	// initialize the second sentence
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	// now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_sentence2, "Font Engine", 300, 200, 1.f, 1.f, 0.f, deviceContext);
	if (!result)
	{
		return false;
	}

	// initialize the thired sentence
	result = InitializeSentence(&m_sentence3, 16, device);
	if (!result)
	{
		return false;
	}

	// now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_sentence3, "Font Engine", 300, 350, 1.f, 1.f, 0.f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	// release the sentences
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);
	ReleaseSentence(&m_sentence3);

	// release the font shader object
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = nullptr;
	}

	// release the font object
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = nullptr;
	}

	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	// draw the first sentence
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// draw the second sentence
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// draw the third sentence
	result = RenderSentence(deviceContext, m_sentence3, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long *indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// create a new sentence object
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// initialize the sentence buffers to null
	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;

	// set the maximum length of the sentence
	(*sentence)->maxLength = maxLength;

	// set the number of vertices in the vertex array
	(*sentence)->vertexCount = 6 * maxLength;

	// set the number of indexes in the index array
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// create the vertex array
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// create the index array
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// initialize vertex array to zeros at first
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// initialize the index array
	for (int i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// set up the description of the dynamic vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// give the subresource structure a ptr to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create the vertex buffer 
	result = device->CreateBuffer(
		&vertexBufferDesc, 
		&vertexData, 
		&(*sentence)->vertexBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	// set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
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
		&(*sentence)->indexBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	// release the vertex array
	delete[] vertices;
	vertices = nullptr;

	// release the index array
	delete[] indices;
	indices = nullptr;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text,
	int positionX, int positionY, float red, float green, float blue,
	ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// store the color of the sentence
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// get the number of letters in the sentence
	numLetters = (int)strlen(text);

	// check for possible buffer overflow
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// create the vertex array
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// initialize vertex array to zeros at first
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// calculate the X and Y pixel position on the screen to start drawing to
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// use the font class to build the vertex array from the sentence text and draw location
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// lock the vertex buffer so it can be written to
	result = deviceContext->Map(
		sentence->vertexBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
		);
	if (FAILED(result))
	{
		return false;
	}

	// get a ptr to the data in the vertex buffer 
	verticesPtr = (VertexType*)mappedResource.pData;

	// copy the data into the vertex buffer
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// unlock the vertex buffer
	deviceContext->Unmap(
		sentence->vertexBuffer, 
		0
		);

	// release the vertex array
	delete[] vertices;
	vertices = nullptr;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// release the sentence vertex buffer
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = nullptr;
		}

		// release the sentence index buffer
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = nullptr;
		}

		// release the sentence
		delete *sentence;
		*sentence = nullptr;
	}

	return;
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	XMVECTOR pixelColor;
	bool result;

	// set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	// set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(
		0,
		1,
		&sentence->vertexBuffer,
		&stride,
		&offset
		);

	// set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(
		sentence->indexBuffer,
		DXGI_FORMAT_R32_UINT,
		0
		);

	// set the type of primitive that should be rendered
	deviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);

	// create a pixel color vector in the input sentence color
	pixelColor = XMLoadFloat4(&XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.f));

	// render the text using the font shader
	result = m_FontShader->Render(
		deviceContext,
		sentence->indexCount,
		worldMatrix, m_baseViewMatrix, orthoMatrix,
		m_Font->GetTexture(),
		pixelColor
		);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool TextClass::SetDirection(float x, float y, float z, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char directionString[16];
	bool result;

	// convert the X float to string format
	sprintf_s(tempString, "%.1f", x);

	// setup the directionX string
	strcpy_s(directionString, "");
	strcat_s(directionString, tempString);

	// convert the Y float to string format
	sprintf_s(tempString, "%.1f", y);

	// setup the directionY string
	strcat_s(directionString, "/");
	strcat_s(directionString, tempString);

	// convert the Y float to string format
	sprintf_s(tempString, "%.1f", z);

	// setup the directionY string
	strcat_s(directionString, "/");
	strcat_s(directionString, tempString);

	// update the sentence vertex buffer with the new string information
	result = UpdateSentence(
		m_sentence1,
		directionString,
		20, 20,
		1.f, 1.f, 1.f,
		deviceContext
	);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetValuef(float x, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char directionString[16];
	bool result;

	// convert the X float to string format
	sprintf_s(tempString, "%.1f", x);

	// setup the directionX string
	strcpy_s(directionString, "");
	strcat_s(directionString, tempString);

	// update the sentence vertex buffer with the new string information
	result = UpdateSentence(
		m_sentence3,
		directionString,
		20, 60,
		1.f, 1.f, 1.f,
		deviceContext
	);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetValuei(int x, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char directionString[16];
	bool result;

	// convert the X float to string format
	sprintf_s(tempString, "%i", x);

	// setup the directionX string
	strcpy_s(directionString, "");
	strcat_s(directionString, tempString);

	// update the sentence vertex buffer with the new string information
	result = UpdateSentence(
		m_sentence3,
		directionString,
		20, 60,
		1.f, 1.f, 1.f,
		deviceContext
	);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetKeyPressed(unsigned char* key, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char keyString[16];
	bool result;

	if (key[DIK_A] & 0x80)
	{
		strcpy_s(tempString, "A");
	}
	else if (key[DIK_S] & 0x80)
	{
		strcpy_s(tempString, "S");
	}
	else if (key[DIK_D] & 0x80)
	{
		strcpy_s(tempString, "D");
	}
	else if (key[DIK_Q] & 0x80)
	{
		strcpy_s(tempString, "Q");
	}
	else if (key[DIK_W] & 0x80)
	{
		strcpy_s(tempString, "W");
	}
	else if (key[DIK_E] & 0x80)
	{
		strcpy_s(tempString, "E");
	}
	else
	{
		strcpy_s(tempString, "");
	}

	// setup the key string
	strcpy_s(keyString, "Key: ");
	strcat_s(keyString, tempString);

	// update the sentence vertex buffer with the new string information
	result = UpdateSentence(
		m_sentence2,
		keyString,
		20, 40,
		1.f, 1.f, 1.f,
		deviceContext
	);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char fpsString[16];
	float red, green, blue;
	bool result;

	// truncate the fps to below 10'000
	if (fps > 9999)
	{
		fps = 9999;
	}

	// convert the fps integer to string format
	_itoa_s(fps, tempString, 10);

	// setup the fps string
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// if fps is 60 or above set the fps color to green
	if (fps >= 60)
	{
		red = 0.f;
		green = 1.f;
		blue = 0.f;
	}

	// if fps is below 60 set the fps color to yellow
	if (fps < 60)
	{
		red = 1.f;
		green = 1.f;
		blue = 0.f;
	}

	// if fps is below 30 set the fps color to red
	if (fps < 30)
	{
		red = 1.f;
		green = 0.f;
		blue = 0.f;
	}

	// update the sentence vertex buffer with the new string information
	result = UpdateSentence(
		m_sentence1,
		fpsString,
		20, 20,
		red, green, blue,
		deviceContext
	);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;

	// convert the cpu integer to string format
	_itoa_s(cpu, tempString, 10);

	// setup the cpu string
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// update the sentence vertex buffer with the new string information
	result = UpdateSentence(
		m_sentence2,
		cpuString,
		20, 40,
		1.f, 1.f, 0.f,
		deviceContext
	);
	if (!result)
	{
		return false;
	}

	return true;
}