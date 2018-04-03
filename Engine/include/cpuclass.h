#ifndef CPUCLASS_H
#define CPUCLASS_H

#pragma comment(lib, "pdh.lib")

#include <Pdh.h>

class CpuClass
{
public:
	CpuClass() = default;
	CpuClass(const CpuClass&) = default;
	~CpuClass() = default;
	// rule of five
	CpuClass& operator=(const CpuClass&) = default;
	CpuClass(CpuClass&&) = default;
	CpuClass& operator=(CpuClass&&) = default;

	void Initialize();
	void Shutddown();
	void Frame();
	int GetCpuPercentage();

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};

#endif	// CPUCLASS_H