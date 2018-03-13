#ifndef LIGHTSHADERCLASS_H
#define LIGHTSHADERCLASS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fstream>

class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMVECTOR ambientColor;
		XMVECTOR diffuseColor;
		XMFLOAT3 lightDirection;
		float specularPower;
		XMVECTOR specularColor;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};


public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();
	// rule of five
	LightShaderClass& operator=(const LightShaderClass&) = default;
	LightShaderClass(LightShaderClass&&) = default;
	LightShaderClass& operator=(LightShaderClass&&) = default;

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX,
		ID3D11ShaderResourceView*, XMFLOAT3, XMVECTOR, XMVECTOR, XMFLOAT3, XMVECTOR, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX,
		ID3D11ShaderResourceView*, XMFLOAT3, XMVECTOR, XMVECTOR, XMFLOAT3, XMVECTOR, float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};

#endif	// LIGHTSHADERCLASS_H