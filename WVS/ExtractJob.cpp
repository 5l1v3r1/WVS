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
	WriteFile("��ַ��.txt" + to_string(m_pWorkThread->getThreadID()), m_pItem->getUrl() + string("\tMethod:") + (char)(m_pItem->getMethod() + '0') + string("\tlayer:") + to_string(m_pItem->getLayer()) + ("\tAEGUMENT:") + vecFieldToString2(m_pItem->getArgs()) + ("\tcookie:") +  tempCookie.toString() );
	m_pWorkThread->m_curlCode = m_pWorkThread->m_pHttpClient->send(m_pItem->getMethod(), tempCookie.toString(), m_pItem->getUrl(), m_pItem->getArgs(), strHtml);
//	WriteFile("��ַ������" + to_string(m_pWorkThread->getThreadID())+".html", strHtml);
	switch (m_pWorkThread->m_curlCode)
	{
		/*
		CURLE_OK: printf("send ok!\n");
		CURLE_HTTP_POST_ERROR: printf("post error!\n");
		CURLE_COULDNT_CONNECT: printf("cannot connect to server\n");
		*/
		case CURLE_OK:
		{
						 status = m_pWorkThread->m_pHttpClient->getStatusCode();
						 WriteFile("��ַ��_2.txt", "curle_ok");
						 //ֻ������ȷ������£��Ž�����ȡͷ��body
						 if ((status / 100) == 3)
						 {
							 //�ض�λ����header��ȡ��ַ��
							 WriteFile("��ַ��_2.txt", "curle_ok");
							 Item* tempItem = m_pData->analyseRedirectHeader(m_pItem,m_pWorkThread->m_strHeader);
							 if (tempItem != NULL)
							 {
								 pJob = new CExtractJob(tempItem, m_pData, m_pTest);
								 m_pWorkThread->m_pThreadPool->addJob(pJob, NULL);
							 }
						 }

						 m_pData->analyseHeader(m_pWorkThread->m_strHeader);
						 vector<Item*> *pItemVec = m_pData->analyseHtml(m_pItem, strHtml);
						 //������з�����Ϻ�Ĵ���
						
						 for (unsigned int i = 0; i < pItemVec->size(); i++)
						 {
							 pJob = new CExtractJob((*pItemVec)[i], m_pData, m_pTest);
							 _cprintf("addNewJob%s\n", (*pItemVec)[i]->getUrl().c_str());
							 m_pWorkThread->m_pThreadPool->addJob(pJob, NULL);
						 }
						 delete pItemVec;
						 _cprintf("TestItem��%s\n", m_pItem->getUrl().c_str());
						 if (m_pItem->getArgs().size()>0)
						 {
							 m_pTest->test(m_pWorkThread->m_pHttpClient, m_pItem);
						 }
						 cout << "��ȡ���!" << endl;

						 break;
		}
		case CURLE_COULDNT_CONNECT:{	//�޷����ӷ�����
									   WriteLog("CURLE_COULDNT_CONNECT:" + m_pItem->getUrl());
									   break;
		}
		default:{
					WriteFile("��ַ��_2.txt", std::to_string(m_pWorkThread->m_curlCode));
				   break;
		}
	}
}
