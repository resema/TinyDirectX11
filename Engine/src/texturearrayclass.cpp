#include "texturearrayclass.h"
#include "DDSTextureLoader.h"


TextureArrayClass::TextureArrayClass()
{
	m_textures[0] = nullptr;
	m_textures[1] = nullptr;
	m_textures[2] = nullptr;
}

bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2, WCHAR* filename3)
{
	HRESULT result;

	// load the first texture in
	result = DirectX::CreateDDSTextureFromFile(
		device,
		filename1,
		NULL,
		&m_textures[0]
	);
	if (FAILED(result))
	{
		return false;
	}

	// load the second texture in
	result = DirectX::CreateDDSTextureFromFile(
		device,
		filename2,
		NULL,
		&m_textures[1]
	);
	if (FAILED(result))
	{
		return false;
	}


	// load the third texture in
	result = DirectX::CreateDDSTextureFromFile(
		device,
		filename3,
		NULL,
		&m_textures[2]
	);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureArrayClass::Shutdown()
{
	// release the texture resources
	if (m_textures[0])
	{
		m_textures[0]->Release();
		m_textures[0] = nullptr;
	}

	if (m_textures[1])
	{
		m_textures[1]->Release();
		m_textures[1] = nullptr;
	}

	if (m_textures[2])
	{
		m_textures[2]->Release();
		m_textures[2] = nullptr;
	}

	return;
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_textures;
}
