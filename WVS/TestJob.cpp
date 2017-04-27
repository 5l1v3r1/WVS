#include "stdafx.h"
#include "TestJob.h"

TestJob::TestJob(CHttpClient *pHttpClient, Item*pItem, CData*pData, CSQLiTest*pSQLiTest)
{
	m_pHttpClient = pHttpClient;
	m_pItem = pItem;
	m_pData = pData;
	m_pTest = pSQLiTest;
}


TestJob::~TestJob()
{
}

void TestJob::Run(void*ptr)
{
	
	m_pTest->test(m_pHttpClient, m_pItem);
}
