#include "inputclass.h"

InputClass::InputClass()
	: m_directInput(nullptr), m_keyboard(nullptr), m_mouse(nullptr)
{
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// store the screen size which will be used for positioning the mosue cursor
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// initialize the location of the mouse on the screen
	m_mouseX = 0;
	m_mouseY = 0;

	// initialize the main direct input interface
	result = DirectInput8Create(
		hinstance,					// instance handle
		DIRECTINPUT_VERSION,		// DirectInput version nbr
		IID_IDirectInput8,			// Unique identifier of the desired interface
		(void**)&m_directInput,		// ptr to variable to receive the interface ptr
		NULL						// ptr to address of controlling object
		);
	if (FAILED(result))
	{
		return false;
	}

	// initialize the direct input interface for the KEYBOARD
	result = m_directInput->CreateDevice(
		GUID_SysKeyboard,		// ref to the GUID for desired input device
		&m_keyboard,			// addr of variable to receive interface ptr
		NULL					// addr of controlling object
		);
	if (FAILED(result))
	{
		return false;
	}

	// set the data format. in this case since it is a keyboard we can use the predefined data format
	result = m_keyboard->SetDataFormat(
		&c_dfDIKeyboard			// addr of struct that describes format of data
		);
	if (FAILED(result))
	{
		return false;
	}

	// set the cooperative level of the keyboard to NOT SHARE with other programs
	result = m_keyboard->SetCooperativeLevel(
		hwnd,									// application window
		DISCL_FOREGROUND | DISCL_EXCLUSIVE		// requested level
		);
	if (FAILED(result))
	{
		return false;
	}

	// now ACQUIRE the keyboard
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	//
	// initialize the direct input interface for the MOUSE
	result = m_directInput->CreateDevice(
		GUID_SysMouse,			// ref to the GUID for desired input device
		&m_mouse,				// addr of variable to receive interface ptr
		NULL					// addr of controlling object
		);
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetDataFormat(
		&c_dfDIMouse			// addr of struct that describes format of data
		);
	if (FAILED(result))
	{
		return false;
	}

	// set the cooperative level of the mouse to SHARE with other programs
	result = m_mouse->SetCooperativeLevel(
		hwnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
		);
	if (FAILED(result))
	{
		return false;
	}

	// ACQUIRE the mouse
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	// release the mouse
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = nullptr;
	}

	// release the keyboard
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = nullptr;
	}

	// release the main interface to direct input
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = nullptr;
	}
}

bool InputClass::Frame()
{
	bool result;

	// read the current state of the keyboard
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// read the current state of the mouse
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// process the changes in the mouse and keyboard
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// read the keyboard device
	result = m_keyboard->GetDeviceState(
		sizeof(m_keyboardState),		// size of the buffer of the data, in bytes
		(LPVOID)&m_keyboardState		// data ; addr of struct that receives the state
		);
	if (FAILED(result))
	{
		// if the keyboard lost focus or was not acquired then try to get control back
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	// read the mouse device
	result = m_mouse->GetDeviceState(
		sizeof(m_mouseState),			// size of the buffer of the data ; in bytes
		(LPVOID)&m_mouseState			// data ; addr of struct that receives the state
		);
	if (FAILED(result))
	{
		// if the mouse lost focus or was not acquired then try to get control back
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	// update the location of the mouse cursor based on the change during the frame
	m_mouseX = m_mouseState.lX;
	m_mouseY = m_mouseState.lY;

	// update the key pressed on the keyboard during the frame
	memcpy_s(m_key, sizeof(m_key), m_keyboardState, sizeof(m_keyboardState));

	return;
}

bool InputClass::IsEscapePressed()
{
	// do a bitwise AND on the keyboard state to check if the escape key is pressed
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;

	return;
}

void InputClass::GetKeyPressed(unsigned char** key)
{
	*key = m_key;

	return;
}