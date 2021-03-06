#include "systemclass.h"

SystemClass::SystemClass()
	: m_Input(nullptr), m_Graphics(nullptr), 
	  m_Fps(nullptr), m_Cpu(nullptr), 
	  m_Timer(nullptr), m_Position(nullptr)
{
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// initialize the width and height of the screen to zero before sending the variables into the function
	screenWidth = 0;
	screenHeight = 0;

	// initialize the windows api
	InitializeWindows(screenWidth, screenHeight);

	//create the input object. This obj will be used to handle reading the keyboard input from the user
	m_Input = new InputClass();
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (FAILED(result))
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// create the graphics object. this obj will handle rendering al the graphics for this application
	m_Graphics = new GraphicsClass();
	if (!m_Graphics)
	{
		return false;
	}

	// initialize the graphics obj
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	//create the fps object
	m_Fps = new FpsClass;
	if (!m_Fps)
	{
		return false;
	}
	// initialize the fps object
	m_Fps->Initialize();

	// create the cpu object
	m_Cpu = new CpuClass;
	if (!m_Cpu)
	{
		return false;
	}
	// initialize the cpu object
	m_Cpu->Initialize();

	// create the timer object
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}
	// intialize the timer object
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer objet.", L"Error", MB_OK);
		return false;
	}

	// create the position object
	m_Position = new PositionClass;
	if (!m_Position)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// release the position object
	if (m_Position)
	{
		delete m_Position;
		m_Position = nullptr;
	}

	// release the timer object
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = nullptr;
	}

	// release the cpu object
	if (m_Cpu)
	{
		delete m_Cpu;
		m_Cpu = nullptr;
	}

	// release the fps object
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = nullptr;
	}

	// Release the graphics object
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	// Release the input obj
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = nullptr;
	}

	// shutdown the window
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// initialize the msg structure
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user
	done = false;
	while (!done)
	{
		// Handle the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translating like keyboard's virtual key to char
			DispatchMessage(&msg);		// sends message to our procedure WndProcd
		}

		// if windows signals to end the application then exit out
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// otherwise do the frame processing
			result = Frame();
			if (!result)
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				done = true;
			}
		}

		// check if the user pressed escape and wants to quit
		if (m_Input->IsEscapePressed() == true)
		{
			done = true;
		}
	}

	return;
}

bool SystemClass::Frame()
{
	bool result;
	int mouseX, mouseY;
	unsigned char* key = nullptr;

	// update the system stats
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// do the input frame processing
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// get the location of the mouse from the input object
	m_Input->GetMouseLocation(mouseX, mouseY);

	// get the key pressed from the input object
	m_Input->GetKeyPressed(&key);

	// set the frame time for calculating the updated position
	m_Position->SetFrameTime(m_Timer->GetTime());

	// update viewing angle by passing the mouse position
	m_Position->SetMousePosition(mouseX, mouseY);

	// update position by passing the keyboard input
	m_Position->SetKeyboard(key);

	// do the position update
	result = m_Position->Frame();
	if (!result)
	{
		return false;
	}

	// do the frame processing for the graphics obj
	result = m_Graphics->Frame(
		m_Fps->GetFps(),
		m_Cpu->GetCpuPercentage(),
		m_Timer->GetTime(),
		m_Position->GetPosition(), 
		m_Position->GetDirection(), 
		m_Position->GetUp()
	);
	if (!result)
	{
		return false;
	}

	// finally render the graphics to the screen
	result = m_Graphics->Render();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// get an external pointer to this obj
	ApplicationHandle = this;

	// get the instance of this application
	m_hinstance = GetModuleHandle(NULL);

	// give the application a name
	m_applicationName = L"Engine";

	// setup the windows class with default settings
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;		// window procedure
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// register the window class
	RegisterClassEx(&wc);

	// determine the resolution of the clients desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// setup the screen settings depending on fullscreen or not
	if (FULL_SCREEN)
	{
		// set the size to maximum and 32bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// change the display settings to fullscreen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// set the position of the window to top left corner
		posX = posY = 0;
	}
	else {
		screenWidth = 640;
		screenHeight = 480;

		// place the window in the middle of screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// create the window and get the handle to it
	m_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW, 
		m_applicationName,									// class name
		m_applicationName,									// window name
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, 
		screenWidth, screenHeight, 
		NULL,												// handle to parent wnd
		NULL,												// handle to attached menu
		m_hinstance, 
		NULL
		);

	// bring the window up on screen and set it as main focus
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// hide the mouse cursor
	if (FULL_SCREEN) {
		ShowCursor(false);
	}
	else {
		ShowCursor(true);
	}
	return;
}

void SystemClass::ShutdownWindows()
{
	// show the mouse cursor
	ShowCursor(true);

	// Fix the display settings if leaving full screen
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// remove the application instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// release the pointer to this class
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	// check if the window is being destroyed
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	
	// check if the window is being closed
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// pass to the message handler in the systemclass
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}