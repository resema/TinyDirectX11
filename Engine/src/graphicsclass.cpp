#include "graphicsclass.h"

#include <string>

GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
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

	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
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
	// release the Direct3D object
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
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
	// clear the buffers to begin the scene
	m_Direct3D->BeginScene(1.f, 1.f, 0.f, 1.f);

	// present the rendered scene to the screen
	m_Direct3D->EndScene();

	return true;
}