#include "cpuclass.h"

void CpuClass::Initialize()
{
	PDH_STATUS status;

	// initialize the flag indicating wether this object can read the system cpu usage or not
	m_canReadCpu = true;

	// create a query object to poll cpu usage
	status = PdhOpenQuery(
		NULL,				// data source, e.g. log file
		0,					// user-defined value to retrieve the data later
		&m_queryHandle		// handle to the query
	);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	// set query object to poll all cpus in the system
	status = PdhAddEnglishCounter(
		m_queryHandle,									// query
		TEXT("\\Processor(_Total)\\% processor time"),	// counter path
		0,												// user-defined value
		&m_counterHandle								// handle to the counter
	);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	m_lastSampleTime = GetTickCount();

	m_cpuUsage = 0;

	return;
}

void CpuClass::Shutddown()
{
	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	}
	return;
}

void CpuClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu)
	{
		if ((m_lastSampleTime + 1000) < GetTickCount())
		{
			m_lastSampleTime = GetTickCount();
			
			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(
				m_counterHandle,	// handle of the coutner
				PDH_FMT_LONG,		// data type of the formatte value
				NULL,				// counter type
				&value				// struct that receives the counter value
			);

			m_cpuUsage = value.longValue;
		}
	}
	return;
}

int CpuClass::GetCpuPercentage()
{
	int usage;

	if (m_canReadCpu)
	{
		usage = (int)m_cpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}

