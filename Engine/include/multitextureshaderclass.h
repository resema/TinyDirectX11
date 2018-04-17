#ifndef MULTITEXTURESHADERCLASS_H
#define MULTITEXTURESHADERCLASS_H

#include "d3dclass.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fstream>

class MultiTextureShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	MultiTextureShaderClass();
	MultiTextureShaderClass(const MultiTextureShaderClass&) = default;
	~MultiTextureShaderClass() = default;
	// rule of five
	MultiTextureShaderClass& operator=(const MultiTextureShaderClass&) = default;
	MultiTextureShaderClass(MultiTextureShaderClass&&) = default;
	MultiTextureShaderClass& operator=(MultiTextureShaderClass&&) = default;

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_samplerState;
};

#endif	// MULTITEXTURESHADERCLASS_H