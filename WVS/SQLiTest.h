#pragma once
#include <string>
#include "tinystr.h"
#include "tinyxml.h"
#include <iostream>
#include "Item.h"
#include "CHttpClient.h"
#include "Data.h"
#include "Test.h"
#include <math.h>

using namespace std;
#ifndef TEST_CASE
#define TEST_CASE
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
	string identify;	//无用
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
typedef struct SQLiResult{
	int id;
	int caseId;
	int itemId;
	int injectPos;
	int resultState;
	int type;
	string url;
	string cookie;
	string args;
	string ext;	//用于基于bool的注入记录详情； 格式为。caseId:length;
}SQLiResult;
#endif


class CSQLiTest:public Test
{
public:
	CSQLiTest(CData* pData);
	~CSQLiTest();
	BOOL loadConfiguration(string fileName = "SQLiTestCase.xml");
	BOOL saveConfiguration(string fileName = "SQLiTestCase.xml");
	void insertEBC(ErrorBasedCase *pEBC, TiXmlElement *EBCRoot);
	void insertBBC(BoolBasedCase *pBBC, TiXmlElement *BBCRoot);
	void insertTBC(TimeBasedCase *pTBC, TiXmlElement *TBCRoot);
	bool test(CHttpClient *pHttpClient, Item *pItem);
	bool errorBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime);
	bool boolBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime);
	bool timeBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long averageTime);

	string resultToString();
private:
	BOOL m_errorBased = TRUE;
	BOOL m_boolBased = TRUE;
	BOOL m_timeBased = TRUE;
	vector<ErrorBasedCase*> EBCvec;
	vector<TimeBasedCase*>TBCvec;
	vector<BoolBasedCase*>BBCvec;
	vector<SQLiResult*>resultVec;
	CData* m_pData;
	long m_lateTime = 1000;	//允许意外的网络延迟时间（ms）

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

