#pragma once
#include <string>
#include "tinyxml2.h"
#include <iostream>
#include "Item.h"
#include "CHttpClient.h"
#include "Data.h"
#include "Test.h"
#include <math.h>
#include "TestResult.h"

using namespace std;

typedef struct ErrorBasedCase{
	int id;
	string inject;
	string identify;
	string check;
}ErrorBasedCase, *PErrorBasedCase;
typedef struct BoolBasedCase{
	int id;
	string inject;
	string inject2;	//无用
	string identify;	
	string check;		//无用
}BoolBasedCase, *PBoolBasedCase;
typedef struct TimeBasedCase{
	int id;
	string inject;
	string injectPost;
	string identify;
	string identifyPost;
	double waitTime;
}TimeBasedCase, *PTimeBasedCase;



class CSQLiTest:public Test
{
public:
	CSQLiTest(CData* pData, TestManager* pTestManger);
	~CSQLiTest();
	bool loadConfiguration(string fileName = "SQLiTestCase.xml");
	bool saveConfiguration(string fileName = "SQLiTestCase.xml");
	void insertEBC(ErrorBasedCase *pEBC, tinyxml2::XMLElement *EBCRoot, tinyxml2::XMLDocument* myDocument);
	void insertBBC(BoolBasedCase *pBBC, tinyxml2::XMLElement *BBCRoot, tinyxml2::XMLDocument* myDocument);
	void insertTBC(TimeBasedCase *pTBC, tinyxml2::XMLElement *TBCRoot, tinyxml2::XMLDocument* myDocument);
	bool test(CHttpClient *pHttpClient, Item *pItem);
	bool errorBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime);
	bool boolBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime);
	bool timeBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long averageTime);
	void setTestMode(bool errorBased, bool boolBased, bool timeBased);

private:
	BOOL m_errorBased = TRUE;
	BOOL m_boolBased = TRUE;
	BOOL m_timeBased = TRUE;
	vector<ErrorBasedCase*> m_vecEBTestCase;
	vector<TimeBasedCase*> m_veerTBTestCase;
	vector<BoolBasedCase*> m_vecBBTestCase;
	long m_lateTime = 10000;	//允许意外的网络延迟时间（ms）

	BOOL htmlEqual(string html, string html2);
	string getComment(HttpMethod method);
	CURLcode send(CHttpClient*pHttpClient,
				  HttpMethod method,
				  string cookieStr,
				  string url,
				  string args,
				  string& html,
				  long&sumTime,
				  int&sendCount);
};

