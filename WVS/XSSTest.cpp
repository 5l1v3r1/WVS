#include "stdafx.h"
#include "XSSTest.h"
#include "conio.h"
#include "TestManager.h"

XSSTest::XSSTest(CData* pData, TestManager* pTestManager)
{
	
	m_pData = pData;
	m_pTestManager = pTestManager;
	//loadConfiguration();
}

XSSTest::~XSSTest()
{

}

bool XSSTest::loadConfiguration(string fileName /*= "XSSConf.xml"*/)
{
	tinyxml2::XMLDocument *myDocument = new tinyxml2::XMLDocument();
	if (myDocument->LoadFile(fileName.c_str()) == tinyxml2::XML_ERROR_FILE_NOT_FOUND)
	{
		_cprintf("文件不存在\n");
		WriteLog("SQLiTest LoadConfiguration failed: file noe exist:" + fileName);
		return false;
	}

	tinyxml2::XMLElement* RootElement = myDocument->RootElement();

	if (RootElement == NULL)
	{
		_cprintf("%s file is empty\n", fileName.c_str());
		return false;
	}
	else{
		_cprintf("%s\n", RootElement->Value());
	}

	PXSSTestCase pTestCase;
	tinyxml2::XMLElement *testCaseEle = RootElement->FirstChildElement();
	while (testCaseEle != NULL){
		pTestCase = new XSSTestCase();
		testCaseEle->QueryIntAttribute("id", &(pTestCase->id));
		tinyxml2::XMLElement *injectEle = testCaseEle->FirstChildElement();
		tinyxml2::XMLElement *identiEle = injectEle->NextSiblingElement();
		if (injectEle->GetText() == 0){
			pTestCase->inject = " ";
		}
		else{
			pTestCase->inject = injectEle->GetText();
		}
		pTestCase->identify = identiEle->FirstChild()->Value();
		_cprintf("id:%d\tinject:%s\tidentify:%s\n", pTestCase->id, pTestCase->inject.c_str(), pTestCase->identify.c_str());
		m_vecTestCase.push_back(pTestCase);
		testCaseEle = testCaseEle->NextSiblingElement();
	}
	return true;
}


bool XSSTest::saveConfiguration(string fileName /*= "XSSConf.xml"*/)
{
	//创建一个XML的文档对象。
	tinyxml2::XMLDocument *myDocument = new tinyxml2::XMLDocument();
	//创建一个根元素并连接。

	tinyxml2::XMLElement *RootElement = myDocument->NewElement("XSSTestCase");
	myDocument->LinkEndChild(RootElement);
	XSSTestCase testCase = { 0, "<sCriPt \r\n>alert('RandomNumForReplace')</ScriPt> ", "<script\\s*>alert\\('RandomNumForReplace'\\)</script>"};
	insertTestCase(&testCase, RootElement, myDocument);
	testCase = { 0, "<bOdy \r\n onlOaD=\"alert('RandomNumForReplace')\">", "<body\\s*onload=\"alert\\('RandomNumForReplace'\\)\">" };
	insertTestCase(&testCase, RootElement, myDocument);
	testCase = { 0, "<img src=\"\" onerror=\"alert('RandomNumForReplace')\"/>", "<img src=\"\" onerror=\"alert\\('RandomNumForReplace'\\)\"/>" };
	insertTestCase(&testCase, RootElement, myDocument);
	testCase = { 0, "<a href=\"javascript:alert('RandomNumForReplace')\">xxx</a>", "<a href=\"javascript:alert\\('RandomNumForReplace'\\)\">xxx</a>" };
	insertTestCase(&testCase, RootElement, myDocument);
	testCase = { 0, "<a hRef=\"data:text/html;base64,PHNjcmlwdD5hbGVydCgnYmFzZTY0IGVuY3J5cHRvJyk8L3NjcmlwdD4=\">xxx</a>", 
		"<a hRef=\"data:text/html;base64,PHNjcmlwdD5hbGVydCgnYmFzZTY0IGVuY3J5cHRvJyk8L3NjcmlwdD4=\">xxx</a" };
	insertTestCase(&testCase, RootElement, myDocument);
	testCase = { 0, "<ifrAMe src=\"data:text/html;base64,PHNjcmlwdD5hbGVydCgnYmFzZTY0IGVuY3J5cHRvJyk8L3NjcmlwdD4=\">",
		"<ifrAMe src=\"data:text/html;base64,PHNjcmlwdD5hbGVydCgnYmFzZTY0IGVuY3J5cHRvJyk8L3NjcmlwdD4=\">" };
	insertTestCase(&testCase, RootElement, myDocument);
	testCase = { 0, "<scRi<script>pT \r\n>alert('RandomNumForReplace')</ScRi<script>pT>", "<script\\s*>alert\\('RandomNumForReplace'\\)</script>" };
	insertTestCase(&testCase, RootElement, myDocument); 
	myDocument->SaveFile(fileName.c_str());//保存到文件
	return true;
}

void XSSTest::insertTestCase(XSSTestCase *pTestCase, tinyxml2::XMLElement *RootEle, tinyxml2::XMLDocument* myDocument)
{
	int i = -1;
	tinyxml2::XMLElement *ele = (tinyxml2::XMLElement*)RootEle->LastChild();
	if (ele == NULL)
	{
		i = 0;
	}
	else
	{
		ele->QueryIntAttribute("id", &i);
	}
	pTestCase->id = i + 1;
	tinyxml2::XMLElement* caseEle = myDocument->NewElement("TestCase");
	tinyxml2::XMLElement* injectEle = myDocument->NewElement("inject");
	tinyxml2::XMLElement* identifyEle = myDocument->NewElement("identify");
	caseEle->LinkEndChild(injectEle);
	caseEle->LinkEndChild(identifyEle);
	tinyxml2::XMLText *injectText = myDocument->NewText(pTestCase->inject.c_str());
	tinyxml2::XMLText *identifyText = myDocument->NewText(pTestCase->identify.c_str());
	injectEle->LinkEndChild(injectText);
	identifyEle->LinkEndChild(identifyText);
	caseEle->SetAttribute("id", pTestCase->id);
	RootEle->LinkEndChild(caseEle);
}

bool XSSTest::test(CHttpClient *pHttpClient, Item *pItem)
{
	if (m_useXSS)
	{
		//开始测试过程
		string html = "";
		Cookie cookie;
		string cookieStr;
		string args = "";
		TestResult* pResult = NULL;
		m_pData->getCookie(cookie);
		cookieStr = cookie.toString();
		HttpMethod method = pItem->getMethod();
		string url = pItem->getUrl();
		CURLcode code;
		long randomNum;
		string identify;
		unsigned posOfRandom = -1;

		int resultState = 0;	//1 表示只有inject有，2表示两个都有。
		vector<string> vecRes;

		bool flag = false;
		for (unsigned pos = 0; pos < pItem->getArgs().size(); pos++)
		{

			if ((((pItem->getArgs())[pos].getSecurityFlag()) & 2) != 0)	//这个参数是否需要测试
				continue;

			for (unsigned i = 0; i < m_vecTestCase.size(); i++)
			{
				resultState = 0;
				html = "";
				randomNum = clock() * 10000 + rand();

				//参数编制。
				args = pItem->getArgsStr(pos, m_vecTestCase[i]->inject, false);
				if ((posOfRandom = args.find("RandomNumForReplace")) != -1)
					args.replace(posOfRandom, 19, to_string(randomNum));
				code = pHttpClient->send(method, cookieStr, url, args, html);
				if (code == CURLE_OK)
				{
					if (pHttpClient->getStatusCode() / 100 == 5)
					{
						//返回码为服务器内部错误，则认为有漏洞。
						resultState = 4500;
					}
					else if (pHttpClient->getStatusCode() / 100 == 4)
					{
						//	resultState = 1000 + pHttpClient->getStatusCode();
						continue;
					}
					else
					{
						identify = m_vecTestCase[i]->identify;
						if ((posOfRandom = identify.find("RandomNumForReplace")) != -1)
							identify.replace(posOfRandom, 19, to_string(randomNum));
						findByRegex(html, identify, vecRes, false);
						if (vecRes.size() > 0)
						{
							resultState = 401;
							vecRes.clear();
						}
					}
				}
				if (resultState != 0)
				{
					//记录情况
					pResult = new TestResult();
					pResult->itemId = pItem->getId();
					pResult->url = pItem->getUrl();
					pResult->caseId = m_vecTestCase[i]->id;
					pResult->injectPos = pos;
					pResult->cookie = cookie.toString();
					pResult->args = pItem->getArgsStr();
					pResult->resultState = resultState;
					pResult->type = 4;
					pResult->argStrs = pItem->getArgsStr(pos, m_vecTestCase[i]->inject, false, false);
					pResult->method = method;
					/*if (resultState == 401)
					{
					pResult->vecResponse.push_back(html);
					}*/
					m_pTestManager->putResultItem(pResult);
					pItem->getArgs()[pos].setResultId(pResult->id);
					WriteFile("网址_结果格式化.txt", generateResult(pResult->id, pResult->resultState, pResult->url, pResult->method, pResult->args, pResult->argStrs, m_pTestManager->g_separator));
					break;
				}
			}
			if (resultState > 0)
				flag = true;
		}
		return flag;
	}
	return false;
}

void XSSTest::setTestMode(bool useXSS)
{
	m_useXSS = useXSS;
}
