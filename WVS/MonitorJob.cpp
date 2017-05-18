#include "stdafx.h"
#include "MonitorJob.h"
#include "Data.h"

MonitorJob::MonitorJob(HWND hwnd, clock_t start, ThreadPool* pThreadPool)
{
	m_start = start;
	m_pThreadPool = pThreadPool;
	//m_pData = pData;
	m_hwnd = hwnd;
}

MonitorJob::~MonitorJob()
{

}

void MonitorJob::Run(void*ptr) 
{
	int waitTime = 1000;
	while (true)
	{
		Sleep(waitTime);
		m_now = clock();
		if ((m_pThreadPool->getBusyWorkerNum() == 1) && (m_pThreadPool->getRestJobNum() == 0)){
			SendMessage(m_hwnd, WM_MY_MONITOR, 0, (m_now - m_start) / CLOCKS_PER_SEC);
			break;
		}
		else{
			SendMessage(m_hwnd, WM_MY_MONITOR, 1, (m_now - m_start) / CLOCKS_PER_SEC);
		}
		waitTime = 1000;
	}
}
