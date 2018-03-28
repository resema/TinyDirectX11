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

//
// globals
const bool VCARD_INFO = false;
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float	SCREEN_NEAR = 0.1f;


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

	bool Frame(int, int);
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
};

#endif	// GRAPHICSCLASS_H