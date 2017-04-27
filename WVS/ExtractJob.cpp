#include "stdafx.h"
#include "ExtractJob.h"


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


CExtractJob::CExtractJob(Item*pItem, CData* pData, Test*pTest)
{
	m_pData = pData;
	m_pItem = pItem;
	m_pTest = pTest;
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
	WriteFile("网址树.txt" + to_string(m_pWorkThread->getThreadID()), m_pItem->getUrl() + string("\tMethod:") + (char)(m_pItem->getMethod() + '0') + string("\tlayer:") + to_string(m_pItem->getLayer()) + ("\tAEGUMENT:") + vecFieldToString2(m_pItem->getArgs()) + ("\tcookie:") + tempCookie.toString());
	
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
						pJob = new CExtractJob(tempItem, m_pData, m_pTest);
						m_pWorkThread->m_pThreadPool->addJob(pJob, NULL);
					}
				}

				m_pData->analyseHeader(m_pWorkThread->m_strHeader);
				pItemVec = m_pData->analyseHtml(m_pItem, strHtml);
				//这里进行分析完毕后的处理。  将新获取的item都创建任务

				for (unsigned int i = 0; i < pItemVec->size(); i++)
				{
					pJob = new CExtractJob((*pItemVec)[i], m_pData, m_pTest);
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

	//测试网址部分。
	_cprintf("TestItem：%s\n", m_pItem->getUrl().c_str());
	if (m_pItem->getArgs().size() > 0)
	{
		m_pTest->test(pHttpClient, m_pItem);
	}

}
