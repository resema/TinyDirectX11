#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include <DirectXMath.h>
using namespace DirectX;

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetDirection(float, float, float);
	void SetUp(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_directionX, m_directionY, m_directionZ;
	float m_upX, m_upY, m_upZ;
	XMMATRIX m_viewMatrix;
};

#endif	// CAMERACLASS_H