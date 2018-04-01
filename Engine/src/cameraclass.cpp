#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.f;
	m_positionY = 0.f;
	m_positionZ = 0.f;

	m_rotationX = 0.f;
	m_rotationY = 0.f;
	m_rotationZ = 0.f;

	m_directionX = 0.f;
	m_directionY = 0.f;
	m_directionZ = 1.f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
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

void CameraClass::SetDirection(float x, float y, float z)
{
	m_directionX = x;
	m_directionY = y;
	m_directionZ = z;
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

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// setup the vector that points upwards
	up.x = 0.f;
	up.y = 1.f;
	up.z = 0.f;

	// load it into a XMVECTOR structure
	upVector = XMLoadFloat3(&up);

	// setup the position of the camera in the world
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// load it into a XMVECTOR structure
	positionVector = XMLoadFloat3(&position);

	// setup where the camera is looking at by default
	lookAt.x = m_directionX;
	lookAt.y = m_directionY;
	lookAt.z = m_directionZ;

	// load it into a XMVECTOR structure
	lookAtVector = XMLoadFloat3(&lookAt);

	// set the yaw (y-axis), pitch (x-axis), roll (z-axis) rotations in radians
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// create the rotation matrix from the yaw, pitch and roll values
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	
	// transform the lookAt and up vector by the rotation matrix so
	//  the view is correcty rotated at the origin
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// translate the rotated camera position to the location of the viewer
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// finally create the view matrix from the three updated vectors
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}