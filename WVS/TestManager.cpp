#include "stdafx.h"
#include "TestManager.h"


TestManager::TestManager(CData *pData)
{
	
	m_pData = pData;
	m_pSQLiTest = new CSQLiTest(pData);
	m_pXSSTest = new XSSTest(pData);
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

void TestManager::test(CHttpClient *pHttpClient, Item*pItem)
{
	m_pSQLiTest->test(pHttpClient, pItem);
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
	return  m_pSQLiTest->resultToString() + m_pXSSTest->resultToString();
}

std::string TestManager::resultToStringForCSV()
{
	return  m_pSQLiTest->resultToStringForCSV() + m_pXSSTest->resultToStringForCSV();
}

bool TestManager::saveConfiguration(string fileName /*= "XSSConf.xml"*/)
{
	m_pSQLiTest->saveConfiguration();
	m_pXSSTest->saveConfiguration();
	return true;
}

void TestManager::clearResult()
{
	m_pSQLiTest->clearResult();
	m_pXSSTest->clearResult();
}

void TestManager::setTestMode(bool errorBased, bool boolBased, bool timeBased)
{
	m_pSQLiTest->setTestMode(errorBased, boolBased, timeBased);
}
