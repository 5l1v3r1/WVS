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


CExtractJob::CExtractJob(Item* pItem, CData *pData)
{
	m_pData = pData;
	m_pItem = pItem;
}

CExtractJob::~CExtractJob()
{
}

void CExtractJob::Run(void *ptr)
{
	string strHtml = "";
	Cookie tempCookie;
	m_pData->getCookie(tempCookie);
	tempCookie.toString();
	WriteFile("网址树.txt" + to_string(m_pWorkThread->getThreadID()), m_pItem->getUrl() + string("\tMethod:") + (char)(m_pItem->getMethod() + '0') + string("\tlayer:") + to_string(m_pItem->getLayer()) + ("\tAEGUMENT:") + vecFieldToString2(m_pItem->getArgs()) + ("\tcookie:") +  tempCookie.toString() );
	m_pWorkThread->m_curlCode = m_pWorkThread->m_pHttpClient->send(m_pItem->getMethod(), tempCookie.toString(), m_pItem->getUrl(), m_pItem->getArgs(), strHtml);
	WriteFile("网页内容.html", strHtml);
	switch (m_pWorkThread->m_curlCode)
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
						 CExtractJob *pJob;
						 for (unsigned int i = 0; i < pItemVec->size(); i++)
						 {
							 pJob = new CExtractJob((*pItemVec)[i], m_pData);
							 _cprintf("addNewJob%s\n", (*pItemVec)[i]->getUrl().c_str());
							 m_pWorkThread->m_pThreadPool->addJob(pJob, NULL);
						 }
						 delete pItemVec;
						 _cprintf("TestItem：%s\n", m_pItem->getUrl().c_str());
						 cout << "提取完毕!" << endl;
						 break;
		}
		case CURLE_COULDNT_CONNECT:{	//无法连接服务器
									   WriteLog("CURLE_COULDNT_CONNECT:" + m_pItem->getUrl());
									   break;
		}
		default:{
					WriteFile("网址树_2.txt", std::to_string(m_pWorkThread->m_curlCode));
				   break;
		}
	}
}
