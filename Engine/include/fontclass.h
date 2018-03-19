#ifndef FONTCLASS_H
#define FONTCLASS_H

#include <fstream>

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "textureclass.h"

class FontClass
{
private:
	struct FontType
	{
		float left, right;		// TU texture coordinates
		int size;				// size of character in pixel
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();
	// rule of five
	FontClass& operator=(const FontClass&) = default;
	FontClass(FontClass&&) = default;
	FontClass& operator=(FontClass&&) = default;

	bool Initialize(ID3D11Device*, char*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureClass* m_Texture;
};


#endif	// FONTCLASS_H