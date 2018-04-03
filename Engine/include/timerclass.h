#ifndef TIMERCLASS_H
#define TIMERCLASS_H

#include <Windows.h>

class TimerClass
{
public: 
	TimerClass() = default;
	TimerClass(const TimerClass&) = default;
	~TimerClass() = default;
	// rule of five
	TimerClass& operator=(const TimerClass&) = default;
	TimerClass(TimerClass&&) = default;
	TimerClass& operator=(TimerClass&&) = default;

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};

#endif	// TIMERCLASS_H