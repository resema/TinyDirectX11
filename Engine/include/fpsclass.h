#ifndef FPSCLASS_H
#define FPSCLASS_H

#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>

class FpsClass
{
public:
	FpsClass() = default;
	FpsClass(const FpsClass&) = default;
	~FpsClass() = default;
	// rule of five
	FpsClass& operator=(const FpsClass&) = default;
	FpsClass(FpsClass&&) = default;
	FpsClass& operator=(FpsClass&&) = default;

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_fps, m_count;
	unsigned long m_startTime;
};

#endif	// FPSCLASS_H