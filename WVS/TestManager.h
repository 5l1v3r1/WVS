#pragma once
#include <vector>
#include <map>
#include <string>
#include "Test.h"
#include "SQLiTest.h"
#include "XSSTest.h"
#include "Data.h"
using namespace std;
//typedef Test* (__cdecl *NewInstanceProc)(void);


class TestManager
{
public:
	TestManager(Data *pData);
	~TestManager();
	/*bool loadLibByName();
	bool unloadAllLib();*/
	void test(CHttpClient *pHttpClient, Item*pItem, const string&testName);
	bool test(CHttpClient *pHttpClient, Item*pItem);
	bool loadConfiguration(string fileName = "testConf.xml");
	bool saveConfiguration(string fileName = "testConf.xml");
	void setTestMode(bool errorBased, bool boolBased, bool timeBased, bool xssTest);
	void putResultItem(TestResult* pResult);
	vector<TestResult*> getVecpResult() const { return m_vecpResult; }
	string resultToString();
	string resultToStringForCSV();
	string resultToStringFormat();
	string resultToStringFormatById(int resultId);
	void clearResult();
	string g_separator = "^";	//目前只支持一个字符
private:
	//vector<string> m_vecModuleName;
	//map<string, Test*> m_mapNameTest;
	//vector<HINSTANCE> m_vechLib;
	SQLiTest* m_pSQLiTest;
	XSSTest* m_pXSSTest;
	Data* m_pData;
	SRWLOCK m_resultSRW;
	vector<TestResult*> m_vecpResult;
};
