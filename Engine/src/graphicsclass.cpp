#include "graphicsclass.h"

#include <string>

GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_ColorShader = nullptr;
	m_TextureShader = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// create the Direct3D object
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// initialize the Direct3D object
	result = m_Direct3D->Initialize(
		screenWidth, 
		screenHeight, 
		VSYNC_ENABLED, 
		hwnd, 
		FULL_SCREEN, 
		SCREEN_DEPTH, 
		SCREEN_NEAR
		);

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// create the camera object
	m_Camera = new CameraClass;
	if (!m_Camera) {
		return false;
	}

	// set the initial position of the camera
	m_Camera->SetPosition(0.f, 0.f, -5.f);

	// create the model object
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}

	// initialize the model object
	result = m_Model->Initialize(
		m_Direct3D->GetDevice(), 
		m_Direct3D->GetDeviceContext(),
		"../Engine/data/stone01.tga"
		);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	
	// create the texture shader object
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// initialize the texture shader object
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	if (VCARD_INFO)
	{
		char cardName[128];
		int cardMem;
		m_Direct3D->GetVideoCardInfo(cardName, cardMem);
		int len;
		int slength = 128;
		len = MultiByteToWideChar(CP_ACP, 0, cardName, slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, cardName, slength, buf, len);
		std::wstring wsCardName(buf);
		wsCardName.append(L": ");
		wsCardName.append(std::to_wstring(cardMem));
		wsCardName.append(L"Mb");
		delete[] buf;

		int msgboxID = MessageBox(
			NULL,
			(LPCWSTR)wsCardName.c_str(),
			(LPCWSTR)L"Video card name",
			MB_ICONINFORMATION | MB_OK
			);
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// release the texture shader object
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}

	// release the model object
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// release the camera object
	if (m_Camera) {
		delete m_Camera;
		m_Camera = nullptr;
	}

	// release the Direct3D object
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	// render the graphics scene
	result = Render();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// clear the buffers to begin the scene
	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	// generate the view matrix based on the camera's position
	m_Camera->Render();

	// get the world, view and projection matrices from the camera and d3d objects
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// put model vertex and index buffers on the graphics pipeline to prepare them for drawing
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// render the model using the texture shader
	result = m_TextureShader->Render(
		m_Direct3D->GetDeviceContext(), 
		m_Model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture()
		);
	if (!result) {
		return false;
	}

	// present the rendered scene to the screen
	m_Direct3D->EndScene();

	return true;
}