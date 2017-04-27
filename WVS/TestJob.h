#pragma once
#include "CHttpClient.h"
#include "MyThreadPool.h"
#include "Item.h"
#include "Data.h"
#include "SQLiTest.h"

class TestJob :public CJob
{
public:
	TestJob(CHttpClient *pHttpClient, Item*pItem, CData*pData, CSQLiTest*pSQLiTest);
	~TestJob();
	void Run(void*ptr);
private:
	Item* m_pItem;
	CData* m_pData;
	Test* m_pTest;
	CHttpClient *m_pHttpClient;
};

