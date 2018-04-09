#ifndef FRUSTUMCLASS_H
#define FRUSTUMCLASS_H

#include <DirectXMath.h>
using namespace DirectX;

class FrustumClass
{
public:
	FrustumClass() = default;
	FrustumClass(const FrustumClass&) = default;
	~FrustumClass() = default;
	// rule of five
	FrustumClass& operator=(const FrustumClass&) = default;
	FrustumClass(FrustumClass&&) = default;
	FrustumClass& operator=(FrustumClass&&) = default;

	void ConstructFrustum(float, XMMATRIX, XMMATRIX);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

private:
	XMVECTOR m_planes[6];

private:
	float distanceToPlane(int, XMFLOAT3);
};

#endif	// FRUSTUMCLASS_H