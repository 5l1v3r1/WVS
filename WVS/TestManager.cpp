#include "stdafx.h"
#include "TestManager.h"


TestManager::TestManager(Data *pData)
{
	
	m_pData = pData;
	m_pSQLiTest = new SQLiTest(pData, this);
	m_pXSSTest = new XSSTest(pData, this);
	InitializeSRWLock(&m_resultSRW);
	loadConfiguration();
}


TestManager::~TestManager()
{
	/*for (map<string, Test*>::iterator iter = m_mapNameTest.begin(); iter != m_mapNameTest.end(); iter++)
	{
	if (iter->second != NULL)
	delete iter->second;
	}
	unloadAllLib();*/
	if (m_pXSSTest != NULL)
		delete m_pXSSTest;
	if (m_pSQLiTest != NULL)
		delete m_pSQLiTest;
}

//void TestManager::test(CHttpClient *pHttpClient, Item*pItem, const string& testName)
//{
//	if (testName != "")
//	{
//		map<string, Test*>::iterator iter = m_mapNameTest.find(testName);
//		if (iter != m_mapNameTest.end())
//		{
//			iter->second->test(pHttpClient, pItem);
//		}
//		
//	}
//	else{
//		for (map<string, Test*>::iterator iter = m_mapNameTest.begin(); iter != m_mapNameTest.end(); iter++)
//		{
//			iter->second->test(pHttpClient, pItem);
//		}
//	}
//}

bool TestManager::test(CHttpClient *pHttpClient, Item*pItem)
{
	static clock_t sqlTotalTime = 0;
	static clock_t xssTotalTime = 0;
	bool flag = false;
	clock_t start = clock();
	if (m_pSQLiTest->test(pHttpClient, pItem))
	{
		flag = true;
	}
	clock_t sqltime = clock();
	if (m_pXSSTest->test(pHttpClient, pItem))
	{
		flag = true;
	}
	clock_t xsstime = clock();
	sqlTotalTime += (sqltime - start);
	xssTotalTime += (xsstime - sqltime);
	_cprintf("sqlTime:%d\t     xssTime:%d\n", sqlTotalTime, xssTotalTime);
	return flag;
}


////************************************
//// Returns:   bool  只要有一个加载成功就返回true
//// Function:  加载动态库，并将Test实例放进map中。
////************************************
//bool TestManager::loadLibByName()
//{
//	bool flag = false;
//	pair<string, Test*> tempPair;
//	for (unsigned i = 0; i < m_vecModuleName.size(); i++)
//	{
//		string strName = m_vecModuleName[i];
//		strName += ".dll";
//		HINSTANCE hTest = LoadLibrary(StrToCStr(strName));
//		if (hTest != NULL)
//		{
//			NewInstanceProc newInstanceFunc=(NewInstanceProc) GetProcAddress(hTest, m_vecModuleName[i].c_str());
//			m_mapNameTest.insert(pair<string, Test*>(m_vecModuleName[i], newInstanceFunc()));
//			flag = true;
//			m_vechLib.push_back(hTest);
//		}
//		else{
//			WriteLog("load dll fail:" + m_vecModuleName[i]);
//		}
//	}
//	return flag;
//}
//
//bool TestManager::unloadAllLib()
//{
//	for (unsigned i = 0; i < m_vechLib.size(); i++)
//	{
//		FreeLibrary(m_vechLib[i]);
//	}
//	return true;
//}

bool TestManager::loadConfiguration(string fileName /*= "testConf.xml"*/)
{
	/*string test1 = "SQLiTest";
	string test2 = "XSSTest";
	m_vecModuleName.push_back(test1);
	m_vecModuleName.push_back(test2);*/
	m_pSQLiTest->loadConfiguration();
	m_pXSSTest->loadConfiguration();
	return true;
}

std::string TestManager::resultToString()
{
	string args = "\r\nresultId\ttype\tresultState\tcaseId\tinjectPos\tmethod\targs\t\t\t\t\turl------------------------\r\n";
	Item *pItem;
	for (unsigned i = 0; i < m_vecpResult.size(); i++)
	{
		pItem = m_pData->getItemByIndex(m_vecpResult[i]->itemId);
		args += to_string(m_vecpResult[i]->id) +
			"\t\t" + to_string(m_vecpResult[i]->type) +
			"\t" + to_string(m_vecpResult[i]->resultState) +
			"\t\t" + to_string(m_vecpResult[i]->caseId) +
			"\t" + to_string(m_vecpResult[i]->injectPos) +
			"\t" + to_string(m_vecpResult[i]->method) +
			"\t\t" + m_vecpResult[i]->args +
			"\t\t\t\t\t" + m_vecpResult[i]->url +
			"\r\n";
	}
	return args;
}

std::string TestManager::resultToStringForCSV()
{
	string args = "resultId, type, resultState, caseId, injectPos, method, args,url, cookie, argStr\n";
	string html = "";
	unsigned i;
	for ( i = 0; i < m_vecpResult.size(); i++)
	{
		args += to_string(m_vecpResult[i]->id) +
			"," + to_string(m_vecpResult[i]->type) +
			"," + to_string(m_vecpResult[i]->resultState) +
			"," + to_string(m_vecpResult[i]->caseId) +
			"," + to_string(m_vecpResult[i]->injectPos) +
			"," + to_string(m_vecpResult[i]->method) +
			"," + m_vecpResult[i]->args +
			"," + m_vecpResult[i]->url +
			"," + m_vecpResult[i]->cookie +
			"," + m_vecpResult[i]->argStrs +
			"\n";
		html = "";
		/*for (j = 0; j < m_vecpResult[i]->vecResponse.size(); j++)
		{
		html += m_vecpResult[i]->vecResponse[j];
		html += "\n-----------------------------------NO."+to_string(j+1)+"----------------------------------------\n";
		}
		WriteFile("网址结果存档_" + to_string(m_vecpResult[i]->id), html);*/
	}
	return args;
}

bool TestManager::saveConfiguration(string fileName /*= "XSSConf.xml"*/)
{
	m_pSQLiTest->saveConfiguration();
	m_pXSSTest->saveConfiguration();
	return true;
}

void TestManager::clearResult()
{
	AcquireSRWLockExclusive(&m_resultSRW);
	m_vecpResult.clear();
	ReleaseSRWLockExclusive(&m_resultSRW);
}

void TestManager::setTestMode(bool errorBased, bool boolBased, bool timeBased, bool xssTest)
{
	m_pSQLiTest->setTestMode(errorBased, boolBased, timeBased);
	m_pXSSTest->setTestMode(xssTest);
}

void TestManager::putResultItem(TestResult* pResult)
{
	AcquireSRWLockExclusive(&m_resultSRW);
	pResult->id = m_vecpResult.size();
	m_vecpResult.push_back(pResult);
	ReleaseSRWLockExclusive(&m_resultSRW);
}

std::string TestManager::resultToStringFormat()
{
	string str = "";
	TestResult *pResult;
	str = "共有" + to_string(m_vecpResult.size()) + "处可能存在漏洞\r\n";
	for (unsigned i = 0; i < m_vecpResult.size(); i++)
	{
		pResult = m_vecpResult[i];
		str += generateResult(pResult->id, pResult->resultState, pResult->url, pResult->method, pResult->args, pResult->argStrs, g_separator);
	}
	return str;
}

std::string TestManager::resultToStringFormatById(int resultId)
{
	TestResult *pResult;
	pResult = m_vecpResult[resultId];
	return generateResult(pResult->id, pResult->resultState, pResult->url, pResult->method, pResult->args, pResult->argStrs, g_separator);
}


