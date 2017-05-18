#pragma once
#include "Test.h"
#include "Tool.h"
#include "Data.h"
#include "tinyxml2.h"

typedef struct XSSTestCase
{
	int id;
	string inject;
	string identify;
}XSSTestCase,*PXSSTestCase;

class XSSTest: public Test
{
public:
	XSSTest(Data* pData, TestManager* pTestManager);
	~XSSTest();
	virtual bool loadConfiguration(string fileName = "XSSConf.xml");
	virtual bool saveConfiguration(string fileName = "XSSConf.xml");
	void insertTestCase(XSSTestCase *pTestCase, tinyxml2::XMLElement *RootEle, tinyxml2::XMLDocument* myDocument);
	virtual bool test(CHttpClient *pHttpClient, Item *pItem);
	void setTestMode(bool useXSS);
private:
	
	vector<XSSTestCase*>m_vecTestCase;
	bool m_useXSS;
};

