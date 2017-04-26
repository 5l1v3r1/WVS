#pragma once
#include "stdafx.h"
#include "MyThreadPool.h"
#include "Item.h"
#include "Data.h"
#include "CHttpClient.h"
#include "Data.h"
#include "Test.h"

class CExtractJob : public CJob
{
public:
	CExtractJob(Item*pItem, CData* pData, Test*pTest);
	~CExtractJob();

	void Run(void *ptr);
private:
	Item* m_pItem;
	CData* m_pData;
	Test* m_pTest;
};
