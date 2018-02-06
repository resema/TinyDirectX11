#include "inputclass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	int i;

	// initialize all keys to being release and not pressed
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
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