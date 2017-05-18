#pragma once
#include "MyThreadPool.h"


class MonitorJob :public Job
{
public:
	MonitorJob(HWND hwnd,clock_t start, ThreadPool* pThreadPool);
	~MonitorJob();
	void Run(void*ptr);
private:
	ThreadPool *m_pThreadPool;
	clock_t m_start;
	clock_t m_now;
	//CData*m_pData;
	HWND m_hwnd;
};

