#ifndef GRAPHICSCLASS_H
#define GRAPHICSCLASS_H

#include "d3dclass.h"

const bool FULLSCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float	SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();

private:
	D3DClass* m_Direct3D;
};

#endif	// GRAPHICSCLASS_H