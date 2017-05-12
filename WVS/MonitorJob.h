#pragma once
#include "MyThreadPool.h"


class MonitorJob :public CJob
{
public:
	MonitorJob(HWND hwnd,clock_t start, CMyThreadPool* pThreadPool);
	~MonitorJob();
	void Run(void*ptr);
private:
	CMyThreadPool *m_pThreadPool;
	clock_t m_start;
	clock_t m_now;
	//CData*m_pData;
	HWND m_hwnd;
};

