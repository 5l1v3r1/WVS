#pragma once
#include <string>
#include "tinyxml2.h"
#include <iostream>
#include "Item.h"
#include "CHttpClient.h"
#include "Data.h"
#include "Test.h"
#include <math.h>

using namespace std;
//#ifndef TEST_CASE
//#define TEST_CASE
typedef struct ErrorBasedCase{
	int id;
	string inject;
	string identify;
	string check;
}ErrorBasedCase, *PErrorBasedCase;
typedef struct BoolBasedCase{
	int id;
	string inject;
	string inject2;	//����
	string identify;	//����
	string check;		//����
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
	string ext;	//���ڻ���bool��ע���¼���飻 ��ʽΪ��caseId:length;
}SQLiResult;
//#endif


class CSQLiTest:public Test
{
public:
	CSQLiTest(CData* pData);
	~CSQLiTest();
	BOOL loadConfiguration(string fileName = "SQLiTestCase.xml");
	BOOL saveConfiguration(string fileName = "SQLiTestCase.xml");
	void insertEBC(ErrorBasedCase *pEBC, tinyxml2::XMLElement *EBCRoot, tinyxml2::XMLDocument* myDocument);
	void insertBBC(BoolBasedCase *pBBC, tinyxml2::XMLElement *BBCRoot, tinyxml2::XMLDocument* myDocument);
	void insertTBC(TimeBasedCase *pTBC, tinyxml2::XMLElement *TBCRoot, tinyxml2::XMLDocument* myDocument);
	bool test(CHttpClient *pHttpClient, Item *pItem);
	bool errorBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime);
	bool boolBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime);
	bool timeBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long averageTime);
	void setTestMode(bool errorBased, bool boolBased, bool timeBased);
	string resultToString();
	string resultToStringForCSV();
	void clearResult();
	void putResultItem( void* pResult);
private:
	BOOL m_errorBased = TRUE;
	BOOL m_boolBased = TRUE;
	BOOL m_timeBased = TRUE;
	vector<ErrorBasedCase*> EBCvec;
	vector<TimeBasedCase*>TBCvec;
	vector<BoolBasedCase*>BBCvec;
	vector<SQLiResult*>resultVec;
	CData* m_pData;
	long m_lateTime = 1000;	//��������������ӳ�ʱ�䣨ms��
	SRWLOCK m_resultSRW;

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

