#pragma once
#include "CThreadPool.h"
#include "ExtractJob.h"
class CScheduleJob : public CJob
{
public:
	CScheduleJob(string oriUrl, CThreadManage *m_pManage);
	~CScheduleJob();
	void Run(void *ptr);
private:
	CThreadManage *m_pManage;
	CData*m_pData;
	CThreadMutex m_webSiteLinksMutex;
	CCondition m_webSiteLinksCondition;
	
};

