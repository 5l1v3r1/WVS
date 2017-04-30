#include "stdafx.h"
#include "XSSTest.h"
#include "conio.h"

XSSTest::XSSTest()
{
	InitializeSRWLock(&m_resultSRW);
}

XSSTest::XSSTest(CData* pData)
{
	InitializeSRWLock(&m_resultSRW);
	m_pData = pData;
}


XSSTest::~XSSTest()
{

}

bool XSSTest::loadConfiguration(string fileName /*= "XSSConf.xml"*/)
{
	WriteLog("XSS加载配置文件");
	return true;
}


bool XSSTest::saveConfiguration(string fileName /*= "XSSConf.xml"*/)
{
	WriteLog("XSS存储配置文件");
	return true;
}


bool XSSTest::test(CHttpClient *pHttpClient, Item *pItem)
{
	return false;
}

void XSSTest::putResultItem(void*ptr)
{
	AcquireSRWLockExclusive(&m_resultSRW);
	WriteLog("增加结果项");
	//m_vecpResult->push_back(new XSSResult);
	ReleaseSRWLockExclusive(&m_resultSRW);
}

std::string XSSTest::resultToString()
{
	string resultStr = "XSS---------------------------------------------\nI am the xss result!";
	return resultStr;
}

std::string XSSTest::resultToStringForCSV()
{
	string resultStr = "XSS\n I am the xss result!  CVS";
	return resultStr;
}

void XSSTest::clearResult()
{
	for (vector<XSSResult*>::iterator iter = m_vecpResult.begin(); iter != m_vecpResult.end(); iter++)
	{
		if (*iter != NULL)
		{
			delete *iter;
		}
	}
}

