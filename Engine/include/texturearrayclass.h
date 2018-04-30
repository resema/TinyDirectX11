#ifndef TEXTUREARRAYCLASS_H
#define TEXTUREARRAYCLASS_H

#include <d3d11.h>

class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&) = default;
	~TextureArrayClass() = default;
	// rule of five
	TextureArrayClass& operator=(const TextureArrayClass&) = default;
	TextureArrayClass(TextureArrayClass&&) = default;
	TextureArrayClass& operator=(TextureArrayClass&&) = default;

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

private:
	ID3D11ShaderResourceView* m_textures[5];
};

#endif	// TEXTUREARRAYCLASS_H