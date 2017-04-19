#include "stdafx.h"
#include "ScheduleJob.h"

CScheduleJob::CScheduleJob(string oriUrl, CThreadManage*pManage)
{
	m_webSiteLinksMutex = CThreadMutex();
	m_webSiteLinksCondition = CCondition();
	m_pManage = pManage;
	m_pData = new CData();
	m_pData->originUrl = oriUrl;
	_cprintf("%s\n",m_pData->originUrl.c_str());
	Item *pItem = new Item(HttpMethod::get, "http://192.168.8.191/DVWA-master/login.php");
	m_pData->crawlerLinksItemVec.push_back(pItem);
	//CExtractJob *eJob = new CExtractJob(pItem, m_pData, m_pManage);
	//m_pManage->Run(eJob, NULL);
}

CScheduleJob::~CScheduleJob()
{

}

void CScheduleJob::Run(void *ptr)
{
//	t_begin = clock();
	Item* pItem;
	bool flag = true;
	while (flag)
	{
		m_webSiteLinksCondition.MyEnterCriticalSection();
		while ((pItem = m_pData->getItem()) == NULL)
		{
			if (m_pManage->getBusyNum() == 0)
			{
				flag = false;
				break;
			}
			m_webSiteLinksCondition.Wait(10000);
		}
		if (pItem == NULL)
		{
			break;
		}
		m_webSiteLinksCondition.MyLeaveCriticalSection();
		CExtractJob *pTempExtract = new CExtractJob(pItem, m_pData);
		pTempExtract->m_isDeleteAfterDone = true;	//动态申请，需要在任务完成后，由外部操作删除（此处为线程run）
		m_pManage->Run(pTempExtract, NULL);
		_cprintf("implements tset function!\n");
	}
}
