#ifndef GRAPHICSCLASS_H
#define GRAPHICSCLASS_H

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "textureshaderclass.h"
#include "lightclass.h"
#include "bitmapclass.h"
#include "textclass.h"
#include "modellistclass.h"
#include "frustumclass.h"


//
// globals
const bool VCARD_INFO = false;
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float	SCREEN_NEAR = 0.1f;
const float STEP = 0.01f;
const float STEP_LRG = 0.1f;


class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();
	// rule of five
	GraphicsClass& operator=(const GraphicsClass&) = default;
	GraphicsClass(GraphicsClass&&) = default;
	GraphicsClass& operator=(GraphicsClass&&) = default;

	bool Initialize(int, int, HWND);
	void Shutdown();

	bool Frame(int, int, float, XMFLOAT3, XMFLOAT3, XMFLOAT3);
	bool Render();

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	LightShaderClass* m_LightShader;
	TextureShaderClass* m_TextureShader;
	LightClass* m_Light;
	BitmapClass* m_Bitmap;
	TextClass* m_Text;
	ModelListClass* m_ModelList;
	FrustumClass* m_Frustum;
};

#endif	// GRAPHICSCLASS_H