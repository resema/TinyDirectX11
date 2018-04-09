#include "frustumclass.h"

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	float zMinimum, r;
	XMMATRIX matrix;
	XMFLOAT4X4 fmatrix, fprojMatrix;

	// store matrices in specific 4x4 floats
	XMStoreFloat4x4(&fprojMatrix, projectionMatrix);

	// calculate the minimum Z distance in the frustum
	zMinimum = -fprojMatrix._43 / fprojMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	fprojMatrix._33 = r;
	fprojMatrix._43 = -r * zMinimum;

	// load 4x4 floats in matrices
	projectionMatrix = XMLoadFloat4x4(&fprojMatrix);

	// create the frustum matrix from the view matrix and updated projection matrix
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	// store matrix in 4x4 float
	XMStoreFloat4x4(&fmatrix, matrix);

	// calculate NEAR plane of frustum
	m_planes[0] = XMLoadFloat4(&XMFLOAT4(
		fmatrix._14 + fmatrix._13,
		fmatrix._24 + fmatrix._23,
		fmatrix._34 + fmatrix._33,
		fmatrix._44 + fmatrix._43
	));
	m_planes[0] = XMPlaneNormalize(m_planes[0]);

	// calculate FAR plane of frustum
	m_planes[1] = XMLoadFloat4(&XMFLOAT4(
		fmatrix._14 - fmatrix._13,
		fmatrix._24 - fmatrix._23,
		fmatrix._34 - fmatrix._33,
		fmatrix._44 - fmatrix._43
	));
	m_planes[1] = XMPlaneNormalize(m_planes[1]);

	// calculate the LEFT plane of frustum
	m_planes[2] = XMLoadFloat4(&XMFLOAT4(
		fmatrix._14 + fmatrix._11,
		fmatrix._24 + fmatrix._21,
		fmatrix._34 + fmatrix._31,
		fmatrix._44 + fmatrix._41
	));
	m_planes[2] = XMPlaneNormalize(m_planes[2]);

	// calculate the RIGHT plane of frustum
	m_planes[3] = XMLoadFloat4(&XMFLOAT4(
		fmatrix._14 - fmatrix._11,
		fmatrix._24 - fmatrix._21,
		fmatrix._34 - fmatrix._31,
		fmatrix._44 - fmatrix._41
	));
	m_planes[3] = XMPlaneNormalize(m_planes[3]);

	// calculate TOP plane of frustum
	m_planes[4] = XMLoadFloat4(&XMFLOAT4(
		fmatrix._14 - fmatrix._12,
		fmatrix._24 - fmatrix._22,
		fmatrix._34 - fmatrix._32,
		fmatrix._44 - fmatrix._42
	));
	m_planes[4] = XMPlaneNormalize(m_planes[4]);

	// calculate BOTTOM plane of frustum
	m_planes[5] = XMLoadFloat4(&XMFLOAT4(
		fmatrix._14 + fmatrix._12,
		fmatrix._24 + fmatrix._22,
		fmatrix._34 + fmatrix._32,
		fmatrix._44 + fmatrix._42
	));
	m_planes[5] = XMPlaneNormalize(m_planes[5]);

	return;
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	// check if the point is inside all six planes of the view frustum
	for (int i = 0; i < 6; i++)
	{
		if (distanceToPlane(i, XMFLOAT3(x, y, z)) < 0.f)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	// check if any one point of the cube is in the view frustum
	for (int i = 0; i < 6; i++)
	{
		if (distanceToPlane(i, XMFLOAT3(xCenter - radius, yCenter - radius, zCenter - radius)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter + radius, yCenter - radius, zCenter - radius)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter - radius, yCenter + radius, zCenter - radius)) >= 0.f)
		{
			continue;
		}
		
		if (distanceToPlane(i, XMFLOAT3(xCenter + radius, yCenter + radius, zCenter - radius)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter - radius, yCenter - radius, zCenter + radius)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter + radius, yCenter - radius, zCenter + radius)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter - radius, yCenter + radius, zCenter + radius)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter + radius, yCenter + radius, zCenter + radius)) >= 0.f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	// check if the radius of the sphere is inside the view frustum
	for (int i = 0; i < 6; i++)
	{
		if (distanceToPlane(i, XMFLOAT3(xCenter, yCenter, zCenter)) < -radius)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	// check if any of the 6 planes of the rectangle are inside the view frustum
	for (int i = 0; i < 6; i++)
	{
		if (distanceToPlane(i, XMFLOAT3(xCenter - xSize, yCenter - ySize, zCenter - zSize)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter + xSize, yCenter - ySize, zCenter - zSize)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter - xSize, yCenter + ySize, zCenter - zSize)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter + xSize, yCenter + ySize, zCenter - zSize)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter - xSize, yCenter - ySize, zCenter + zSize)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter + xSize, yCenter - ySize, zCenter + zSize)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter - xSize, yCenter + ySize, zCenter + zSize)) >= 0.f)
		{
			continue;
		}

		if (distanceToPlane(i, XMFLOAT3(xCenter + xSize, yCenter + ySize, zCenter + zSize)) >= 0.f)
		{
			continue;
		}

		return false;
	}

	return true;
}

//
// helper function
float FrustumClass::distanceToPlane(int plane, XMFLOAT3 pt)
{
	XMFLOAT3 dist;
	// calculate distance between plane and point
	XMVECTOR sum = XMVectorSum(
		XMPlaneDotCoord(
			m_planes[plane],
			XMLoadFloat3(&pt)
		)
	);
	XMStoreFloat3(&dist, sum);
	return dist.x;
}