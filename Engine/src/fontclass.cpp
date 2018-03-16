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