#pragma once
#include "Item.h"
#include "CHttpClient.h"
#include "TestResult.h"
#include "Data.h"
class TestManager;
class Test
{
public:
	Test();
	~Test();
	virtual bool test(CHttpClient *pHttpClient, Item *pItem) = 0;
	virtual bool loadConfiguration(string fileName = "SQLiTestCase.xml")=0;
	virtual bool saveConfiguration(string fileName = "SQLiTestCase.xml")=0;
	//virtual void putResultItem(void*ptr)=0;
	//virtual string resultToString()=0;
	//virtual string resultToStringForCSV()=0;
	//virtual void clearResult()=0;
	TestManager* m_pTestManager;
	Data* m_pData;
	friend class TestManager;
};

