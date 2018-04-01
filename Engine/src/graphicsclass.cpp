#include "graphicsclass.h"

#include <string>
#include <dinput.h>


GraphicsClass::GraphicsClass()
	: position(XMVectorZero()), 
	  posX(0.f), posY(0.f), posZ(0.f), 
	  angleH(0.f), angleV(0.f)
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;

	m_LightShader = nullptr;
	m_TextureShader = nullptr;
	m_Light = nullptr;
	m_Bitmap = nullptr;
	m_Text = nullptr;
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
	XMMATRIX baseViewMatrix;

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
	if (!m_Camera) 
	{
		return false;
	}

	// initialize a base view matrix with the camera for 2D user interface rendering
	m_Camera->SetPosition(0.f, 0.f, -1.f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// set the initial position of the camera
	m_Camera->SetPosition(0.f, 0.f, -3.f);

	// create the model object
	m_Model = new ModelClass;
	if (!m_Model) 
	{
		return false;
	}

	// initialize the model object
	result = m_Model->Initialize(
		m_Direct3D->GetDevice(), 
		m_Direct3D->GetDeviceContext(),
		"./data/model.txt",
		"./data/stone01.tga"
		);
	if (!result) 
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	
	// create the light shader object
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// initialize the light shader object
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// create the texture shader object
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// initialize the light shader object
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// create the light object
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// initialize the light object
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.f);
	m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
	m_Light->SetDirection(1.f, 0.f, 0.f);
	m_Light->SetDirection(1.f, 0.f, 1.f);
	m_Light->SetSpecularColor(1.f, 1.f, 1.f, 1.f);
	m_Light->SetSpecularPower(32.f);

	// create the bitmap object
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}

	// initialize the bitmap object
	result = m_Bitmap->Initialize(
		m_Direct3D->GetDevice(),
		m_Direct3D->GetDeviceContext(),
		screenWidth, screenHeight,
		"./data/stone01.tga", 
		128, 128								// width, height
		);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	// create the text object
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// initialize the text object
	result = m_Text->Initialize(
		m_Direct3D->GetDevice(),
		m_Direct3D->GetDeviceContext(),
		hwnd,
		screenWidth, screenHeight,
		baseViewMatrix
		);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object", L"Error", MB_OK);
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
	// release the text object
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = nullptr;
	}

	// release the bitmap object
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = nullptr;
	}

	// release the light object
	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	// release the light shader object
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	// release the texture shader object
	if (m_TextureShader)
	{
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

bool GraphicsClass::Frame(int mouseX, int mouseY, unsigned char* key)
{
	bool result;

	// update angles
	angleH += mouseX;
	if (angleH > 360.f) { angleH -= 360.f; }
	else if (angleH < 0.f) { angleH += 360.f; }
	angleV += mouseY;
	if (angleV > 360.f) { angleV -= 360.f; }
	else if(angleV < 0.f) { angleV += 360.f; }

	// calculate direction
	XMVECTOR direction = XMLoadFloat3(&XMFLOAT3(
		cosf(angleV * 0.0174532925f) * sinf(angleH * 0.0174532925f),
		-sinf(angleV * 0.0174532925f),
		cosf(angleV * 0.0174532925f) * cosf(angleH * 0.0174532925f)
	));

	// calculate right vector
	XMVECTOR right = XMLoadFloat3(&XMFLOAT3(
		sinf(angleH * 0.0174532925f - 3.14f / 2.f),
		0,
		cosf(angleH * 0.0174532925f - 3.14f / 2.f)
	));

	// calculate the up vector
	XMVECTOR up = XMVector3Cross(right, direction);

	// print the direction
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, up);
	result = m_Text->SetDirection(
		temp.x,
		temp.y,
		temp.z,
		m_Direct3D->GetDeviceContext()
	);
	if (!result)
	{
		return false;
	}

	// set the key pressed
	result = m_Text->SetValuef(
		angleV,
		m_Direct3D->GetDeviceContext()
	);
	if (!result)
	{
		return false;
	}


	// update position
	if (key[DIK_A] & 0x80)		// left
	{
		position += right * STEP;
	}
	if (key[DIK_S] & 0x80)		// back
	{
		position -= direction * STEP;
	}
	if (key[DIK_D] & 0x80)		// right
	{
		position -= right * STEP;
	}
	if (key[DIK_Q] & 0x80)		// down
	{
		position -= up * STEP;
	}
	if (key[DIK_W] & 0x80)		// forward
	{
		position += direction * STEP;
	}
	if (key[DIK_E] & 0x80)		// up
	{
		position += up * STEP;
	}

	// set the direciton of the camera
	XMFLOAT3 tempDirection;
	XMStoreFloat3(&tempDirection, direction);
	m_Camera->SetDirection(tempDirection.x, tempDirection.y, tempDirection.z);

	// set the up of the camera
	XMFLOAT3 tempUp;
	XMStoreFloat3(&tempUp, up);
	m_Camera->SetUp(tempUp.x, tempUp.y, tempUp.z);

	// set the position of the camera
	XMFLOAT3 tempPosition;
	XMStoreFloat3(&tempPosition, position);
	m_Camera->SetPosition(tempPosition.x, tempPosition.y, tempPosition.z);

	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// clear the buffers to begin the scene
	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	// generate the view matrix based on the camera's position
	m_Camera->Render();

	// get the world, view and projection matrices from the camera and d3d objects
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// TURN OFF the z buffer to begin all 2d rendering
	m_Direct3D->TurnZBufferOff();

	// put the bitmap vertex and index buffers on the graphics pipeline
	result = m_Bitmap->Render(
		m_Direct3D->GetDeviceContext(),
		100,		// position x
		100			// position y
		);
	if (!result)
	{
		return false;
	}

	// render the bitmap with the texture shader
	result = m_TextureShader->Render(
		m_Direct3D->GetDeviceContext(),
		m_Bitmap->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix,
		m_Bitmap->GetTexture()
		);
	if (!result)
	{
		return false;
	}

	// TURN ON the Z buffer 
	m_Direct3D->TurnZBufferOn();

	// put model vertex and index buffers on the graphics pipeline to prepare them for drawing
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// render the model using the light shader
	result = m_LightShader->Render(
		m_Direct3D->GetDeviceContext(),
		m_Model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(),
		m_Light->GetDirection(),
		m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(),
		m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower()
		);
	if (!result)
	{
		return false;
	}

	// TURN OFF Z buffer to begin 2D rendering
	m_Direct3D->TurnZBufferOff();

	// TURN ON the alpha blending before rendering the text
	m_Direct3D->TurnOnAlphaBlending();

	// render the text strings
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// TURN OFF the alpha blending
	m_Direct3D->TurnOffAlphaBlending();

	// TURN ON the Z buffer
	m_Direct3D->TurnZBufferOn();

	// present the rendered scene to the screen
	m_Direct3D->EndScene();

	return true;
}