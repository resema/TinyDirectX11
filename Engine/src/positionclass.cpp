#include "positionclass.h"
#include <dinput.h>

PositionClass::PositionClass()
	: m_frameTime(0), m_mouseX(0), m_mouseY(0), m_key(nullptr), m_angleH(0.f), m_angleV(0.f)
{
	m_position = XMVectorZero();
	m_direction = XMVectorZero();
	m_up = XMVectorZero();
	m_right = XMVectorZero();
}

void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}

void PositionClass::SetMousePosition(int mouseX, int mouseY)
{
	m_mouseX = mouseX;
	m_mouseY = mouseY;

	return;
}

void PositionClass::SetKeyboard(unsigned char* key)
{
	m_key = key;
	return;
}

XMFLOAT3 PositionClass::GetDirection()
{
	XMFLOAT3 tempDirection;
	XMStoreFloat3(&tempDirection, m_direction);

	return tempDirection;
}

XMFLOAT3 PositionClass::GetUp()
{
	XMFLOAT3 tempUp;
	XMStoreFloat3(&tempUp, m_up);
	
	return tempUp;
}

XMFLOAT3 PositionClass::GetPosition()
{
	XMFLOAT3 tempPosition;
	XMStoreFloat3(&tempPosition, m_position);

	return tempPosition;
}

void PositionClass::Update()
{
	// update angles
	m_angleH += m_mouseX;
	if (m_angleH > 360.f) 
	{ 
		m_angleH -= 360.f; 
	}
	else if (m_angleH < 0.f) 
	{ 
		m_angleH += 360.f; 
	}
	m_angleV += m_mouseY;
	
	if (m_angleV > 360.f) 
	{ 
		m_angleV -= 360.f; 
	}
	else if (m_angleV < 0.f) 
	{ 
		m_angleV += 360.f; 
	}

	// update position
	if (m_key[DIK_A] & 0x80)		// left
	{
		m_position += m_right * STEP;
	}
	if (m_key[DIK_S] & 0x80)		// back
	{
		m_position -= m_direction * STEP;
	}
	if (m_key[DIK_D] & 0x80)		// right
	{
		m_position -= m_right * STEP;
	}
	if (m_key[DIK_Q] & 0x80)		// down
	{
		m_position -= m_up * STEP;
	}
	if (m_key[DIK_W] & 0x80)		// forward
	{
		m_position += m_direction * STEP;
	}
	if (m_key[DIK_E] & 0x80)		// up
	{
		m_position += m_up * STEP;
	}

}

void PositionClass::Calculate()
{
	// calculate direction
	m_direction = XMLoadFloat3(&XMFLOAT3(
		cosf(m_angleV * 0.0174532925f) * sinf(m_angleH * 0.0174532925f),
		-sinf(m_angleV * 0.0174532925f),
		cosf(m_angleV * 0.0174532925f) * cosf(m_angleH * 0.0174532925f)
	));

	// calculate right vector
	m_right = XMLoadFloat3(&XMFLOAT3(
		sinf(m_angleH * 0.0174532925f - 3.14f / 2.f),
		0,
		cosf(m_angleH * 0.0174532925f - 3.14f / 2.f)
	));

	// calculate the up vector
	m_up = XMVector3Cross(m_right, m_direction);
}