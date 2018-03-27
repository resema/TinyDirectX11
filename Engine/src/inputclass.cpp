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
}

void InputClass::KeyDown(unsigned int input)
{
	// if a key is pressed, save that state
	m_keys[input] = true;
	
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	// if key is release then clear the state
	m_keys[input] = false;

	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	// return state
	return m_keys[key];
}