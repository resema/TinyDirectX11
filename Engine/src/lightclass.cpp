#include "lightclass.h"

LightClass::LightClass()
{
}

LightClass::LightClass(const LightClass& other)
{
}

LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMLoadFloat4(&XMFLOAT4(red, green, blue, alpha));
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMLoadFloat4(&XMFLOAT4(red, green, blue, alpha));
	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = XMLoadFloat4(&XMFLOAT4(red, green, blue, alpha));
	return;
}

void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}

XMVECTOR LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

XMVECTOR LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT3 LightClass::GetDirection()
{
	return m_direction;
}

XMVECTOR LightClass::GetSpecularColor()
{
	return m_specularColor;
}

float LightClass::GetSpecularPower()
{
	return m_specularPower;
}