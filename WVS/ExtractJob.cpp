#include "stdafx.h"
#include "ExtractJob.h"
#include "resource.h"

string vecFieldToString2(vector<Field> fieldVec)
{
	string args;
	for (unsigned int i = 0; i < fieldVec.size(); i++)
	{
		args += fieldVec[i].toString();
		if (i != fieldVec.size() - 1)
		{
			args += "&";
		}
	}
	return args;
}


CExtractJob::CExtractJob(Item*pItem, CData* pData, TestManager*pTestManager, HWND hwnd)
{
	m_pData = pData;
	m_pItem = pItem;
	m_pTestManager = pTestManager;
	m_hwnd = hwnd;
}

CExtractJob::~CExtractJob()
{

}

void CExtractJob::Run(void *ptr)
{
	string strHtml = "";
	Cookie tempCookie;
	int status;
	CExtractJob *pJob;
	m_pData->getCookie(tempCookie);
	CHttpClient*pHttpClient = m_pWorkThread->getHttpClient();
	WriteFile("网址树.txt" + to_string(m_pWorkThread->getThreadID()),
			  to_string(m_pItem->getId()) + "  " +
			  to_string(m_pItem->getOriId()) + " "+
			  m_pItem->getUrl() + 
			  string("\tMethod:") +   (char)(m_pItem->getMethod() + '0') + 
			  string("\tlayer:") + to_string(m_pItem->getLayer()) + 
			  ("\tAEGUMENT:") + /*vecFieldToString2(m_pItem->getArgs())*/ m_pItem->getArgsStr() + 
			  ("\tcookie:") + tempCookie.toString());
	
	//爬取网页部分；	  当当前速度小于指定层次时，才进行下一步爬取。
	if (m_pItem->getLayer() < m_pData->crawlerLayer)
	{
		vector<Item*> *pItemVec = NULL;
		m_pWorkThread->m_curlCode = pHttpClient->send(m_pItem->getMethod(), tempCookie.toString(), m_pItem->getUrl(), m_pItem->getArgs(), strHtml);
		//	WriteFile("网址树内容" + to_string(m_pWorkThread->getThreadID())+".html", strHtml);
		switch (m_pWorkThread->m_curlCode)
		{
			case CURLE_OK:

				status = pHttpClient->getStatusCode();
				WriteFile("网址树_2.txt", "curle_ok");
				//只有在正确的情况下，才进行提取头和body
				if ((status / 100) == 3)	//重定位获得的层次与当前层次相同
				{
					//重定位，从header中取地址。
					WriteFile("网址树_2.txt", "curle_ok");
					Item* tempItem = m_pData->analyseRedirectHeader(m_pItem, m_pWorkThread->m_strHeader);
					if (tempItem != NULL)
					{
						pJob = new CExtractJob(tempItem, m_pData, m_pTestManager, m_hwnd);
						m_pWorkThread->m_pThreadPool->addJob(pJob, NULL);
					}
				}

				m_pData->analyseHeader(m_pWorkThread->m_strHeader);
				pItemVec = m_pData->analyseHtml(m_pItem, strHtml);
				//这里进行分析完毕后的处理。  将新获取的item都创建任务

				for (unsigned int i = 0; i < pItemVec->size(); i++)
				{
					pJob = new CExtractJob((*pItemVec)[i], m_pData, m_pTestManager, m_hwnd);
					_cprintf("addNewJob%s\n", (*pItemVec)[i]->getUrl().c_str());
					m_pWorkThread->m_pThreadPool->addJob(pJob, NULL);
				}
				if (pItemVec != NULL)
				{
					delete pItemVec;
				}
				break;
			case CURLE_COULDNT_CONNECT:	//无法连接服务器
				WriteLog("CURLE_COULDNT_CONNECT:" + m_pItem->getUrl());
				break;

			default:
				WriteFile("网址树_2.txt", std::to_string(m_pWorkThread->m_curlCode));
				break;
		}
	}
	m_pData->addExtractLinkNum();
	//测试网址部分。
	_cprintf("TestItem：%s\n", m_pItem->getUrl().c_str());
	if (m_pItem->getArgs().size() > 0)
	{
		bool flag = false;
		flag = m_pTestManager->test(pHttpClient, m_pItem);
		m_pData->addTestedArgNum(m_pItem->getArgs().size());
		if (flag)
		{
			string * pStr = new string(m_pItem->getUrl());
			SendMessage(m_hwnd, WM_MY_MONITOR, 2, (LPARAM)pStr);
		}
	}
	
	
}
