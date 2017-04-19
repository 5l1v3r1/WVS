#include "stdafx.h"
#include "ExtractJob.h"
#include "CThreadPool.h"
#include "Item.h"
#include "CHttpClient.h"


CExtractJob::CExtractJob(Item* pItem, CData *pData)
{
	m_pData = pData;
	m_pItem = pItem;
	m_pHttpClient = new CHttpClient();
}
CExtractJob::~CExtractJob()
{
	delete m_pHttpClient;
}
void CExtractJob::Run(void *ptr)
{
	string strHtml = "";
	
	m_code = m_pWorkThread->m_pHttpClient->send(m_pItem->getMethod(), m_pData->cookie.toString(), m_pItem->getUrl(), m_pItem->getArgs(), strHtml);
	WriteFile("网页内容.txt", strHtml);
	switch (m_code)
	{
		/*
		CURLE_OK: printf("send ok!\n");
		CURLE_HTTP_POST_ERROR: printf("post error!\n");
		CURLE_COULDNT_CONNECT: printf("cannot connect to server\n");
		*/
		case CURLE_OK:
		{
						 WriteFile("网址树_2.txt", "curle_ok");
						 //只有在正确的情况下，才进行提取头和body
						 m_pData->analyseHeader(m_pWorkThread->m_strHeader);
						 vector<Item*> *pItemVec = m_pData->analyseHtml(m_pItem, strHtml);
						 //这里进行分析完毕后的处理。
						 /* CExtractJob *pJob;
						  for (unsigned int i = 0; i < pItemVec->size(); i++)
						  {
						  pJob = new CExtractJob((*pItemVec)[i], m_pData, m_pThreadManage);
						  _cprintf("addNewJob%s\n", (*pItemVec)[i]->getUrl().c_str());
						  m_pThreadManage->Run(pJob, NULL);
						  }
						  _cprintf("TestItem：%s\n", m_pItem->getUrl().c_str());*/
						 cout << "提取完毕!" << endl;
						 break;
		}
		case CURLE_COULDNT_CONNECT:{	//无法连接服务器
									   WriteLog("CURLE_COULDNT_CONNECT:" + m_pItem->getUrl());
									   break;
		}
		default:{
				   WriteFile("网址树_2.txt", std::to_string(m_code));
				   break;
		}
	}
}
