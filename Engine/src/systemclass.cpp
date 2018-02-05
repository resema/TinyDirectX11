#include "systemclass.h"

SystemClass::SystemClass()
	: m_Input(0), m_Graphics(0)
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
	m_input = new InputClass();
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object
	m_Input->Initialize();

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

	return true;
}

void SystemClass::Shutdown()
{
	// Release the graphics object
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// RElease the input obj
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
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
			TranslateMessage(&msg);
			DispatchMessage(&msg);
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
				done = true;
			}
		}
	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	// check if the user pressed escape and wants to exti the application
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// do the frame processing for the graphics obj
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// check if a key has been pressend on the keyboard
	case WM_KEYDOWN:
	{
		// if a key is pressend send it to the input object so it can record that state
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
	// check if a key has been released on the keyboard
	case WM_KEYUP:
	{
		// if a key is released then send it to the input obj so it can unset the state for that key
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

	// any other message sent to the default message handler
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}