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
	status = PdhAddCounter(
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
				m_counterHandle,
				PDH_FMT_LONG,
				NULL,
				&value
			);

			m_cpuUsage = value.longValue;
		}
	}
	return;
}