#pragma once
#include "stdafx.h"
#include "MyThreadPool.h"
#include "Item.h"
#include "Data.h"
#include "CHttpClient.h"
#include "Data.h"
#include "TestManager.h"


class ExtractJob : public Job
{
public:
	ExtractJob(Item*pItem, Data* pData, TestManager*pTestManager, HWND hwnd);
	~ExtractJob();

	void Run(void *ptr);
private:
	Item* m_pItem;
	Data* m_pData;
	TestManager* m_pTestManager;
	HWND m_hwnd = NULL;
};
