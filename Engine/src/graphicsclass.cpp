#include "graphicsclass.h"

#include <string>

// pre-processing directives
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


GraphicsClass::GraphicsClass()
	//: position(XMVectorZero()), 
	//  posX(0.f), posY(0.f), posZ(0.f), 
	//  angleH(0.f), angleV(0.f)
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_BumpMapShader = nullptr;
	m_Light = nullptr;
	//m_Bitmap = nullptr;
	m_Text = nullptr;
	m_ModelList = nullptr;
	m_Frustum = nullptr;
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
		"./data/sphere.txt",
		L"./data/stone01_conv.dds",
		L"./data/dirt01_conv.dds",
		L"./data/alpha01_conv.dds",
		L"./data/bump01_conv.dds"
		);
	if (!result) 
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	
	// create the light shader object
	m_BumpMapShader = new BumpMapShaderClass;
	if (!m_BumpMapShader)
	{
		return false;
	}

	// initialize the light shader object
	result = m_BumpMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
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
	m_Light->SetDirection(0.f, 0.f, 1.f);
	m_Light->SetSpecularColor(1.f, 1.f, 1.f, 1.f);
	m_Light->SetSpecularPower(32.f);

	//// create the bitmap object
	//m_Bitmap = new BitmapClass;
	//if (!m_Bitmap)
	//{
	//	return false;
	//}

	//// initialize the bitmap object
	//result = m_Bitmap->Initialize(
	//	m_Direct3D->GetDevice(),
	//	m_Direct3D->GetDeviceContext(),
	//	screenWidth, screenHeight,
	//	"./data/stone01.tga", 
	//	128, 128								// width, height
	//	);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
	//	return false;
	//}

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

	// create the model list object
	m_ModelList = new ModelListClass;
	if (!m_ModelList)
	{
		return false;
	}

	// initialize the model list object
	result = m_ModelList->Initialize(500);	// num Models
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// create the frustum object
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
	{
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
	// release the frustum object
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = nullptr;
	}

	// release the model list object
	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = nullptr;
	}

	// release the text object
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = nullptr;
	}

	//// release the bitmap object
	//if (m_Bitmap)
	//{
	//	m_Bitmap->Shutdown();
	//	delete m_Bitmap;
	//	m_Bitmap = nullptr;
	//}

	// release the light object
	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	// release the light shader object
	if (m_BumpMapShader)
	{
		m_BumpMapShader->Shutdown();
		delete m_BumpMapShader;
		m_BumpMapShader = nullptr;
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

bool GraphicsClass::Frame(
	int fps, int cpu, float frameTime,
	XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 up
)
{
	bool result;

	// set the frame per second
	result = m_Text->SetFps(
		fps, 
		m_Direct3D->GetDeviceContext()
	);
	if (!result)
	{
		return false;
	}

	// set the cpu usage
	result = m_Text->SetCpu(
		cpu,
		m_Direct3D->GetDeviceContext()
	);
	if (!result)
	{
		return false;
	}


	// set the direciton of the camera
	m_Camera->SetDirection(direction.x, direction.y, direction.z);

	// set the up of the camera
	m_Camera->SetUp(up.x, up.y, up.z);

	// set the position of the camera
	m_Camera->SetPosition(position.x, position.y, position.z);

	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int modelCount, renderCount;
	float positionX, positionY, positionZ, radius;
	XMFLOAT4 color;
	bool renderModel, result;

	// clear the buffers to begin the scene
	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	// generate the view matrix based on the camera's position
	m_Camera->Render();

	// get the world, view and projection matrices from the camera and d3d objects
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// construct the frustum
	m_Frustum->ConstructFrustum(
		SCREEN_DEPTH,
		projectionMatrix,
		viewMatrix
	);

	// get the number of models that will be rendered
	modelCount = m_ModelList->GetModelCount();

	// initialize the count of models that have been rendered
	renderCount = 0;

	// go through all the models and render them only if they can be seen by the camera
	for (int index = 0; index < modelCount; index++)
	{
		// get the position and color of the object model at this index
		m_ModelList->GetData(
			index,
			positionX, positionY, positionZ,
			color
		);

		// set the radius of the sphere to 1.0 since this is already known
		radius = 1.f;

		// check if the sphere model are in the view frustum
		renderModel = m_Frustum->CheckSphere(
			positionX, positionY, positionZ,
			radius
		);

		// if it can be seen then render it, if not skip this model
		if (renderModel)
		{
			// move the model to the location it should be rendered at
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			// put the model vertex and index buffers on the graphics pipeline
			m_Model->Render(m_Direct3D->GetDeviceContext());

			// render the model using the bumpmap shader
			m_BumpMapShader->Render(
				m_Direct3D->GetDeviceContext(),
				m_Model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTextureArray(),
				m_Light->GetDirection(),
				m_Light->GetAmbientColor(),
				XMLoadFloat4(&color),			// customized color
				m_Camera->GetPosition(),
				m_Light->GetSpecularColor(),
				m_Light->GetSpecularPower()
			);

			// reset tot the original world matrix
			m_Direct3D->GetWorldMatrix(worldMatrix);

			// since this model was rendered, increase the count
			renderCount++;
		}
	}

	// set the number of models that was actually rendered this frame
	result = m_Text->SetValuei(renderCount, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}


	// TURN OFF the z buffer to begin all 2d rendering
	m_Direct3D->TurnZBufferOff();

	//// put the bitmap vertex and index buffers on the graphics pipeline
	//result = m_Bitmap->Render(
	//	m_Direct3D->GetDeviceContext(),
	//	100,		// position x
	//	100			// position y
	//	);
	//if (!result)
	//{
	//	return false;
	//}

	//// render the bitmap with the texture shader
	//result = m_TextureShader->Render(
	//	m_Direct3D->GetDeviceContext(),
	//	m_Bitmap->GetIndexCount(),
	//	worldMatrix, viewMatrix, orthoMatrix,
	//	m_Bitmap->GetTexture()
	//	);
	//if (!result)
	//{
	//	return false;
	//}

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