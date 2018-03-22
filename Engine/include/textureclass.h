#ifndef TEXTURECLASS_H
#define TEXTURECLASS_H

#include <d3d11.h>
#include <stdio.h>

class TextureClass
{
private:
	// targa file header structure
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();
	// rule of five
	TextureClass& operator=(const TextureClass&) = default;
	TextureClass(TextureClass&&) = default;
	TextureClass& operator=(TextureClass&&) = default;

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	bool InitializeDDS(ID3D11DeviceContext*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTarga(char*, int&, int&);

private:
	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
};

#endif	// TEXTURECLASS_H