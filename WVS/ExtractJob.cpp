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
	WriteFile("��ַ��.txt" + to_string(m_pWorkThread->getThreadID()), m_pItem->getUrl() + string("\tMethod:") + (char)(m_pItem->getMethod() + '0') + string("\tlayer:") + to_string(m_pItem->getLayer()) + ("\tAEGUMENT:") + vecFieldToString2(m_pItem->getArgs()) + ("\tcookie:") + tempCookie.toString());
	
	//��ȡ��ҳ���֣�	  ����ǰ�ٶ�С��ָ�����ʱ���Ž�����һ����ȡ��
	if (m_pItem->getLayer() < m_pData->crawlerLayer)
	{
		vector<Item*> *pItemVec = NULL;
		m_pWorkThread->m_curlCode = pHttpClient->send(m_pItem->getMethod(), tempCookie.toString(), m_pItem->getUrl(), m_pItem->getArgs(), strHtml);
		//	WriteFile("��ַ������" + to_string(m_pWorkThread->getThreadID())+".html", strHtml);
		switch (m_pWorkThread->m_curlCode)
		{
			case CURLE_OK:

				status = pHttpClient->getStatusCode();
				WriteFile("��ַ��_2.txt", "curle_ok");
				//ֻ������ȷ������£��Ž�����ȡͷ��body
				if ((status / 100) == 3)	//�ض�λ��õĲ���뵱ǰ�����ͬ
				{
					//�ض�λ����header��ȡ��ַ��
					WriteFile("��ַ��_2.txt", "curle_ok");
					Item* tempItem = m_pData->analyseRedirectHeader(m_pItem, m_pWorkThread->m_strHeader);
					if (tempItem != NULL)
					{
						pJob = new CExtractJob(tempItem, m_pData, m_pTest);
						m_pWorkThread->m_pThreadPool->addJob(pJob, NULL);
					}
				}

				m_pData->analyseHeader(m_pWorkThread->m_strHeader);
				pItemVec = m_pData->analyseHtml(m_pItem, strHtml);
				//������з�����Ϻ�Ĵ���  ���»�ȡ��item����������

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
			case CURLE_COULDNT_CONNECT:	//�޷����ӷ�����
				WriteLog("CURLE_COULDNT_CONNECT:" + m_pItem->getUrl());
				break;

			default:
				WriteFile("��ַ��_2.txt", std::to_string(m_pWorkThread->m_curlCode));
				break;
		}
	}

	//������ַ���֡�
	_cprintf("TestItem��%s\n", m_pItem->getUrl().c_str());
	if (m_pItem->getArgs().size() > 0)
	{
		m_pTest->test(pHttpClient, m_pItem);
	}

}
