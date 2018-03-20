#include "fontclass.h"

FontClass::FontClass()
	: m_Font(nullptr), m_Texture(nullptr)
{
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, char* textureFilename)
{
	bool result;

	// load in the text file containing the font data
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// load the texture that has the font characters on it
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	// release the font texture
	ReleaseTexture();

	// release the font data
	ReleaseFontData();

	return;
}

bool FontClass::LoadFontData(char* filename)
{
	std::ifstream fin;
	char temp;

	// create the font spacing buffer
	m_Font = new FontType[95];		// 95 characters in the texture
	if (!m_Font)
	{
		return false;
	}

	// read in the font size and spacing between chars
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// read in the 95 used ascii characters for text
	for (int i = 0; i < 95; i++)
	{
		fin.get(temp);			// |
		while (temp != ' ')		// |
		{						// |
			fin.get(temp);		// L get Ascii value of character until empty space
		}
		fin.get(temp);			// get the character
		while (temp != ' ')
		{
			fin.get(temp);		// go the start of numeric values
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// close the file
	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	// release the font data array
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = nullptr;
	}

	return;
}

bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// create the texture object
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// initialize the texture object
	result = m_Texture->Initialize(
		device,
		deviceContext,
		filename
		);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::ReleaseTexture()
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

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, idx, letter;

	// coerce the input vertices into a VertexType structure
	vertexPtr = (VertexType*)vertices;

	// get the number of letters in the sentence
	numLetters = (int)strlen(sentence);

	// initialize the index to the vertex array
	idx = 0;

	// draw each letter into a quad
	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// if the letter is a space then just move over three pixels
		if (letter == 0)
		{
			drawX = drawX + 3.f;
		}
		else
		{
			// first triangle in quad
			vertexPtr[idx].position = XMFLOAT3(drawX, drawY, 0.f);	// top left
			vertexPtr[idx].texture = XMFLOAT2(m_Font[letter].left, 0.f);
			idx++;

			vertexPtr[idx].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.f);	// bottom right
			vertexPtr[idx].texture = XMFLOAT2(m_Font[letter].right, 1.f);
			idx++;

			vertexPtr[idx].position = XMFLOAT3(drawX, (drawY - 16), 0.f);	// bottom left
			vertexPtr[idx].texture = XMFLOAT2(m_Font[letter].left, 1.f);
			idx++;

			// second triangle in quad
			vertexPtr[idx].position = XMFLOAT3(drawX, drawY, 0.f);	// top left
			vertexPtr[idx].texture = XMFLOAT2(m_Font[letter].left, 0.f);
			idx++;

			vertexPtr[idx].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.f);	// top right
			vertexPtr[idx].texture = XMFLOAT2(m_Font[letter].right, 0.f);
			idx++;

			vertexPtr[idx].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.f);	// bottom right
			vertexPtr[idx].texture = XMFLOAT2(m_Font[letter].right, 1.f);
			idx++;

			// update the x location for drawing by the size of the letter and one pixel
			drawX = drawX + m_Font[letter].size + 1.f;
		}
	}

	return;
}