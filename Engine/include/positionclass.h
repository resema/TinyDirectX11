#include <math.h>
#include <DirectXMath.h>
using namespace DirectX;

class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&) = default;
	~PositionClass() = default;
	// rule of five
	PositionClass& operator=(const PositionClass&) = default;
	PositionClass(PositionClass&&) = default;
	PositionClass& operator=(PositionClass&&) = default;

	bool Frame();

	void SetFrameTime(float);
	void SetMousePosition(int, int);
	void SetKeyboard(unsigned char*);

	XMFLOAT3 GetDirection();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetPosition();

private:
	void Update();
	void Calculate();

private:
	float m_frameTime;
	int m_mouseX, m_mouseY;
	unsigned char* m_key;
	float m_angleH, m_angleV;

	XMVECTOR m_position, m_direction, m_right, m_up;

private:
	float speed = 0.01f;

};