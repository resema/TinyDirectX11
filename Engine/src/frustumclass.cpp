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

	// calculate near plane of frustum
	m_planes[0] = XMLoadFloat4(&XMFLOAT4(
		fmatrix._14 + fmatrix._13,
		fmatrix._24 + fmatrix._23,
		fmatrix._34 + fmatrix._33,
		fmatrix._44 + fmatrix._43
	));
	
	m_planes[0] = XMPlaneNormalize(m_planes[0]);
}