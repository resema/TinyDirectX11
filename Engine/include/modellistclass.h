#ifndef MODELLISTCLASS_H
#define MODELLISTCLASS_H

#include <stdlib.h>
#include <time.h>
#include <DirectXMath.h>
using namespace DirectX;

class ModelListClass
{
private:
	struct ModelInfoType
	{
		XMFLOAT4 color;
		float positionX, positionY, positionZ;
	};

public:
	ModelListClass();
	ModelListClass(const ModelListClass&) = default;
	~ModelListClass() = default;
	// rule of five
	ModelListClass& operator=(const ModelListClass&) = default;
	ModelListClass(ModelListClass&&) = default;
	ModelListClass& operator=(ModelListClass&&) = default;

	bool Initialize(int);
	void Shutdown();

	int GetModelCount();
	void GetData(int, float&, float&, float&, XMFLOAT4&);

private:
	int m_modelCount;
	ModelInfoType* m_ModelInfoList;
};

#endif	// MODELLISTCLASS_H