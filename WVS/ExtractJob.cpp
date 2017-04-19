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
	WriteFile("��ҳ����.txt", strHtml);
	switch (m_code)
	{
		/*
		CURLE_OK: printf("send ok!\n");
		CURLE_HTTP_POST_ERROR: printf("post error!\n");
		CURLE_COULDNT_CONNECT: printf("cannot connect to server\n");
		*/
		case CURLE_OK:
		{
						 WriteFile("��ַ��_2.txt", "curle_ok");
						 //ֻ������ȷ������£��Ž�����ȡͷ��body
						 m_pData->analyseHeader(m_pWorkThread->m_strHeader);
						 vector<Item*> *pItemVec = m_pData->analyseHtml(m_pItem, strHtml);
						 //������з�����Ϻ�Ĵ���
						 /* CExtractJob *pJob;
						  for (unsigned int i = 0; i < pItemVec->size(); i++)
						  {
						  pJob = new CExtractJob((*pItemVec)[i], m_pData, m_pThreadManage);
						  _cprintf("addNewJob%s\n", (*pItemVec)[i]->getUrl().c_str());
						  m_pThreadManage->Run(pJob, NULL);
						  }
						  _cprintf("TestItem��%s\n", m_pItem->getUrl().c_str());*/
						 cout << "��ȡ���!" << endl;
						 break;
		}
		case CURLE_COULDNT_CONNECT:{	//�޷����ӷ�����
									   WriteLog("CURLE_COULDNT_CONNECT:" + m_pItem->getUrl());
									   break;
		}
		default:{
				   WriteFile("��ַ��_2.txt", std::to_string(m_code));
				   break;
		}
	}
}
