#include "textclass.h"

TextClass::TextClass()
	: m_Font(nullptr), m_FontShader(nullptr),
	  m_sentence1(nullptr), m_sentence2(nullptr)
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
	result = m_Font->Initialize(device, "./data/fontdata.txt", "./data/font.dds");
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
	result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.f, 1.f, 1.f, deviceContext);
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
	result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.f, 1.f, 0.f, deviceContext);
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

	return true;
}