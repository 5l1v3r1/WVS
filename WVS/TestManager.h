#pragma once
#include <vector>
#include <map>
#include <string>
#include "Test.h"
#include "SQLiTest.h"
#include "XSSTest.h"
#include "Data.h"
using namespace std;
typedef Test* (__cdecl *NewInstanceProc)(void);


class TestManager
{
public:
	TestManager(CData *pData);
	~TestManager();
	/*bool loadLibByName();
	bool unloadAllLib();*/
	void test(CHttpClient *pHttpClient, Item*pItem, const string&testName);
	void test(CHttpClient *pHttpClient, Item*pItem);
	bool loadConfiguration(string fileName = "testConf.xml");
	bool saveConfiguration(string fileName = "XSSConf.xml");
	void setTestMode(bool errorBased, bool boolBased, bool timeBased);
	string resultToString();
	string resultToStringForCSV();
	void clearResult();
private:
	//vector<string> m_vecModuleName;
	//map<string, Test*> m_mapNameTest;
	//vector<HINSTANCE> m_vechLib;
	CSQLiTest* m_pSQLiTest;
	XSSTest* m_pXSSTest;
	CData* m_pData;
};

