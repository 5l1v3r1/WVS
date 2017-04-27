#include "stdafx.h"
#include "TestJob.h"

TestJob::TestJob(HWND hwnd, Item*pItem, CData*pData, CSQLiTest*pSQLiTest)
{
	m_pItem = pItem;
	m_pData = pData;
	m_pTest = pSQLiTest;
	m_hwnd = hwnd;
}


TestJob::~TestJob()
{
}

void TestJob::Run(void*ptr)
{
	clock_t start = clock();
	((CSQLiTest*)m_pTest)->clearResult();
	m_pTest->test(m_pWorkThread->getHttpClient(), m_pItem);
	SendMessage(m_hwnd, WM_TEST_JOB, 0, start);
}
