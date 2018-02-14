#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.f;
	m_positionY = 0.f;
	m_positionZ = 0.f;

	m_rotationX = 0.f;
	m_rotationY = 0.f;
	m_rotationZ = 0.f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

