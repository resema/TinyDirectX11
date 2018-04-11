#include "modellistclass.h"

ModelListClass::ModelListClass()
	: m_ModelInfoList(nullptr)
{
}

bool ModelListClass::Initialize(int numModels)
{
	float red, green, blue;
	
	// store the number of models
	m_modelCount = numModels;

	// create a list array of the model information
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	// seed the random generatr with the current time
	srand((unsigned int)time(nullptr));

	// go through all the models and randomly generate the model color and position
	for (int i = 0; i < m_modelCount; i++)
	{
		// generate a random color for the model
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = XMFLOAT4(red, green, blue, 1.f);

		// generate random position in front of the view for the node
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 50.f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 50.f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 50.f) + 5.0f;
	}

	return true;
}

void ModelListClass::Shutdown()
{
	// release the model information list
	if (m_ModelInfoList)
	{
		delete[] m_ModelInfoList;
		m_ModelInfoList = nullptr;
	}

	return;
}

int ModelListClass::GetModelCount()
{
	return m_modelCount;
}

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;

	return;
}