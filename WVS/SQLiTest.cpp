#include "stdafx.h"
#include "SQLiTest.h"
#include "tinyxml2.h"
#include "TestManager.h"

SQLiTest::SQLiTest(Data* pData, TestManager* pTestManager)
{
	m_vecEBTestCase = vector<ErrorBasedCase*>();
	m_veerTBTestCase = vector<TimeBasedCase*>();
	m_vecBBTestCase = vector<BoolBasedCase*>();
	m_pData = pData;
	m_pTestManager = pTestManager;
}

SQLiTest::~SQLiTest()
{

}

bool SQLiTest::loadConfiguration(string fileName /*= "SQLiTestCase.xml"*/)
{
	//读取Xml文件，并遍历
	try
	{
		//创建一个XML的文档对象。

		tinyxml2::XMLDocument *myDocument = new tinyxml2::XMLDocument();
		if (myDocument->LoadFile(fileName.c_str()) == tinyxml2::XML_ERROR_FILE_NOT_FOUND)
		{
			_cprintf("文件不存在\n");
			WriteLog("SQLiTest LoadConfiguration failed: file noe exist:" + fileName);
			return false;
		}

		tinyxml2::XMLElement *RootElement = myDocument->RootElement();

		_cprintf("%s\n", RootElement->Value());

		tinyxml2::XMLElement *EBCRoot = RootElement->FirstChildElement();

		int num = 0;
		EBCRoot->QueryIntAttribute("num", &num);
		PErrorBasedCase pEBC;
		tinyxml2::XMLElement *caseElement = EBCRoot->FirstChildElement();
		for (int i = 0; i < num; i++){
			pEBC = new ErrorBasedCase();
			caseElement->QueryIntAttribute("id", &(pEBC->id));
			tinyxml2::XMLElement *injectElement = caseElement->FirstChildElement();
			tinyxml2::XMLElement *identiEle = injectElement->NextSiblingElement();
			tinyxml2::XMLElement *checkElement = identiEle->NextSiblingElement();
			if (injectElement->GetText() == 0){
				//因tinyxml2 不能存储空格，所做的特殊处理
				pEBC->inject = " ";
			}
			else{
				pEBC->inject = injectElement->GetText();
			}

			pEBC->identify = identiEle->FirstChild()->Value();
			pEBC->check = checkElement->FirstChild()->Value();
			_cprintf("id:%d\tinject:%s\tidentify:%s\tcheck:%s\n", pEBC->id, pEBC->inject.c_str(), pEBC->identify.c_str(), pEBC->check.c_str());
			m_vecEBTestCase.push_back(pEBC);
			caseElement = caseElement->NextSiblingElement();
		}

		tinyxml2::XMLElement *BBCRoot = EBCRoot->NextSiblingElement();

		num = 0;
		BBCRoot->QueryIntAttribute("num", &num);
		PBoolBasedCase pBBC;
		caseElement = BBCRoot->FirstChildElement();
		for (int i = 0; i < num; i++){
			pBBC = new BoolBasedCase();
			caseElement->QueryIntAttribute("id", &(pBBC->id));
			tinyxml2::XMLElement *injectElement = caseElement->FirstChildElement();
			tinyxml2::XMLElement *inject2Element = injectElement->NextSiblingElement();
			tinyxml2::XMLElement *identiEle = inject2Element->NextSiblingElement();
			tinyxml2::XMLElement *checkElement = identiEle->NextSiblingElement();
			pBBC->inject = injectElement->GetText();
			pBBC->inject2 = inject2Element->FirstChild()->Value();
			pBBC->identify = identiEle->FirstChild()->Value();
			pBBC->check = checkElement->FirstChild()->Value();
			_cprintf("id:%d\tinject:%s\tinject2:%s\tidentify:%s\tcheck:%s\n", pBBC->id, pBBC->inject.c_str(), pBBC->inject2.c_str(), pBBC->identify.c_str(), pBBC->check.c_str());
			m_vecBBTestCase.push_back(pBBC);
			caseElement = caseElement->NextSiblingElement();
		}

		tinyxml2::XMLElement *TBCRoot = BBCRoot->NextSiblingElement();

		num = 0;
		TBCRoot->QueryIntAttribute("num", &num);
		PTimeBasedCase pTBC;
		caseElement = TBCRoot->FirstChildElement();
		for (int i = 0; i < num; i++)
		{
			pTBC = new TimeBasedCase();
			caseElement->QueryIntAttribute("id", &(pTBC->id));
			tinyxml2::XMLElement *injectElement = caseElement->FirstChildElement();
			tinyxml2::XMLElement *injectPostElement = injectElement->NextSiblingElement();
			tinyxml2::XMLElement *identiEle = injectPostElement->NextSiblingElement();
			tinyxml2::XMLElement *identiPostEle = identiEle->NextSiblingElement();
			pTBC->inject = injectElement->FirstChild()->Value();
			pTBC->injectPost = injectPostElement->FirstChild()->Value();
			pTBC->identify = identiEle->FirstChild()->Value();
			pTBC->identifyPost = identiPostEle->FirstChild()->Value();
			_cprintf("id:%d\tinject:%s\tinjectpost:%s\tidentify:%s\tidentifyPost:%s\n",
					 pTBC->id, pTBC->inject.c_str(), pTBC->injectPost.c_str(), pTBC->identify.c_str(), pTBC->identifyPost.c_str());
			m_veerTBTestCase.push_back(pTBC);
			caseElement = caseElement->NextSiblingElement();
		}

		myDocument->SaveFile(fileName.c_str());
	}
	catch (string& e)
	{
		WriteLog("SQLiTest LoadConfiguration failed" + e);
		return false;
	}
	return true;
}



bool SQLiTest::saveConfiguration(string fileName /*= "SQLiTestCase.xml"*/)
{
	try
	{
		//创建一个XML的文档对象。
		tinyxml2::XMLDocument *myDocument = new tinyxml2::XMLDocument();
		//创建一个根元素并连接。

		tinyxml2::XMLElement *RootElement = myDocument->NewElement("SQLiTestCase");
		myDocument->LinkEndChild(RootElement);

		tinyxml2::XMLElement *EBCRoot = myDocument->NewElement("ErrorBasedTestCase");
		RootElement->LinkEndChild(EBCRoot);
		EBCRoot->SetAttribute("num", 0);
		EBCRoot->SetAttribute("text", "i am text");
		tinyxml2::XMLElement *BBCRoot = myDocument->NewElement("BoolBasedTestCase");
		RootElement->LinkEndChild(BBCRoot);
		BBCRoot->SetAttribute("num", 0);
		tinyxml2::XMLElement *TBCRoot = myDocument->NewElement("TimeBasedTestCase");
		RootElement->LinkEndChild(TBCRoot);
		TBCRoot->SetAttribute("num", 0);

		ErrorBasedCase EBCcase = { 0, "'", "NULL", "You have an error in your SQL syntax;" };
		insertEBC(&EBCcase, EBCRoot, myDocument);
		ErrorBasedCase EBCcase2 = { 0, "\"", "NULL", "You have an error in your SQL syntax;" };
		insertEBC(&EBCcase2, EBCRoot, myDocument);
		ErrorBasedCase EBCcase3 = { 0, "  ", "NULL", "You have an error in your SQL syntax;" };
		insertEBC(&EBCcase3, EBCRoot, myDocument);

		BoolBasedCase bbcCase = { 0, "' and 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\" and 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "') and 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\") and 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "1524024 and 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "1524024) and 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);

		bbcCase = { 0, "' or 1=1 #", "NULL", "' Or 'ab' = 'ab' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\" or 1=1 #", "NULL", "\" Or 'ab' = 'ab' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "') or 1=1 #", "NULL", "') Or 'ab' = 'ab' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\") or 1=1 #", "NULL", "\") Or 'ab' = 'ab' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "1524024 or 1=1 #", "NULL", "1816807 Or 'ab' = 'ab' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "1524024) or 1=1 #", "NULL", "1816807) Or 'ab' = 'ab' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);

		bbcCase = { 0, "' and 1=2 #", "NULL", "' AND 'abc' = 'bbc' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\" and 1=2 #", "NULL", "\" aND 'abc' = 'bbc' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "') and 1=2 #", "NULL", "') aND 'abc' = 'bbc' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\") and 1=2 #", "NULL", "\") And 'abc' = 'bbc' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "1524024 and 1=2 #", "NULL", "1816807 anD 'abc' = 'bbc' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "1524024) and 1=2 #", "NULL", "1816807) ANd 'abc' = 'bbc' #", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);

		/*TimeBasedCase tbcCase = { 0, "' or if(1=1, sleep(", "), 0) #", "' and if(1=2, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase, TBCRoot, myDocument);
		TimeBasedCase tbcCase2 = { 0, "\" or if(1=1, sleep(", "), 0) #", "\" and if(1=2, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase2, TBCRoot, myDocument);
		TimeBasedCase tbcCase3 = { 0, "') or if(1=1, sleep(", "), 0) #", "') and if(1=2, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase3, TBCRoot, myDocument);
		TimeBasedCase tbcCase4 = { 0, "\") or if(1=1, sleep(", "), 0) #", "\") and if(1=2, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase4, TBCRoot, myDocument);
		TimeBasedCase tbcCase5 = { 0, "23 or if(1=1, sleep(", "), 0) #", "23 or and(1=2, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase5, TBCRoot, myDocument);
		TimeBasedCase tbcCase6 = { 0, "12345) or if(1=1, sleep(", "), 0) #", "12345) and if(1=2, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase6, TBCRoot, myDocument);*/

		TimeBasedCase tbcCase = { 0, "' and if(1=1, sleep(", "), 0) #", "' and if(1=1, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase, TBCRoot, myDocument);
		TimeBasedCase tbcCase2 = { 0, "\" and if(1=1, sleep(", "), 0) #", "\" and if(1=1, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase2, TBCRoot, myDocument);
		TimeBasedCase tbcCase3 = { 0, "') and if(1=1, sleep(", "), 0) #", "') and if(1=1, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase3, TBCRoot, myDocument);
		TimeBasedCase tbcCase4 = { 0, "\") and if(1=1, sleep(", "), 0) #", "\") and if(1=1, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase4, TBCRoot, myDocument);
		TimeBasedCase tbcCase5 = { 0, " and if(1=1, sleep(", "), 0) #", "and if(1=1, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase5, TBCRoot, myDocument);
		TimeBasedCase tbcCase6 = { 0, ") and if(1=1, sleep(", "), 0) #", ") and if(1=1, sleep(0), 0) #", "NULL", 0 };
		insertTBC(&tbcCase6, TBCRoot, myDocument);

		myDocument->SaveFile(fileName.c_str());//保存到文件
	}
	catch (string& e)
	{
		WriteLog("SQLiTest SaveConfiguration failed" + e);
		return false;
	}
	return true;
}
void SQLiTest::insertEBC(ErrorBasedCase *pEBC, tinyxml2::XMLElement *EBCRoot, tinyxml2::XMLDocument* myDocument)
{
	int i = -1;
	tinyxml2::XMLElement *ele = (tinyxml2::XMLElement*)EBCRoot->LastChild();
	if (ele == NULL)
	{
		i = 0;
	}
	else
	{
		ele->QueryIntAttribute("id", &i);
	}

	pEBC->id = i + 1;

	tinyxml2::XMLElement *caseElement = myDocument->NewElement("case");
	EBCRoot->LinkEndChild(caseElement);
	caseElement->SetAttribute("id", pEBC->id);
	tinyxml2::XMLElement *injectElement = myDocument->NewElement("inject");
	tinyxml2::XMLElement *identiEle = myDocument->NewElement("identify");
	tinyxml2::XMLElement *checkElement = myDocument->NewElement("check");
	caseElement->LinkEndChild(injectElement);
	caseElement->LinkEndChild(identiEle);
	caseElement->LinkEndChild(checkElement);

	tinyxml2::XMLText *injectText = myDocument->NewText(pEBC->inject.c_str());
	tinyxml2::XMLText *identiText = myDocument->NewText(pEBC->identify.c_str());
	tinyxml2::XMLText *checkText = myDocument->NewText(pEBC->check.c_str());
	injectElement->LinkEndChild(injectText);
	identiEle->LinkEndChild(identiText);
	checkElement->LinkEndChild(checkText);

	EBCRoot->QueryIntAttribute("num", &i);
	EBCRoot->SetAttribute("num", i + 1);
}
void SQLiTest::insertBBC(BoolBasedCase *pBBC, tinyxml2::XMLElement *BBCRoot, tinyxml2::XMLDocument* myDocument)
{
	int i = -1;
	tinyxml2::XMLElement *ele = (tinyxml2::XMLElement*)BBCRoot->LastChild();
	if (ele == NULL)
	{
		i = 0;
	}
	else
	{
		ele->QueryIntAttribute("id", &i);
	}

	pBBC->id = i + 1;

	tinyxml2::XMLElement *caseElement = myDocument->NewElement("case");
	BBCRoot->LinkEndChild(caseElement);
	caseElement->SetAttribute("id", pBBC->id);
	tinyxml2::XMLElement *injectElement = myDocument->NewElement("inject");
	tinyxml2::XMLElement *inject2Element = myDocument->NewElement("inject2");
	tinyxml2::XMLElement *identiEle = myDocument->NewElement("identify");
	tinyxml2::XMLElement *checkElement = myDocument->NewElement("check");
	caseElement->LinkEndChild(injectElement);
	caseElement->LinkEndChild(inject2Element);
	caseElement->LinkEndChild(identiEle);
	caseElement->LinkEndChild(checkElement);

	tinyxml2::XMLText *injectText = myDocument->NewText(pBBC->inject.c_str());
	tinyxml2::XMLText *inject2Text = myDocument->NewText(pBBC->inject2.c_str());
	tinyxml2::XMLText *identiText = myDocument->NewText(pBBC->identify.c_str());
	tinyxml2::XMLText *checkText = myDocument->NewText(pBBC->check.c_str());
	injectElement->LinkEndChild(injectText);
	inject2Element->LinkEndChild(inject2Text);
	identiEle->LinkEndChild(identiText);
	checkElement->LinkEndChild(checkText);
	BBCRoot->QueryIntAttribute("num", &i);
	BBCRoot->SetAttribute("num", i + 1);
}
void SQLiTest::insertTBC(TimeBasedCase *pTBC, tinyxml2::XMLElement *TBCRoot, tinyxml2::XMLDocument* myDocument)
{
	int i = -1;
	tinyxml2::XMLElement *ele = (tinyxml2::XMLElement*)TBCRoot->LastChild();
	if (ele == NULL)
	{
		i = 0;
	}
	else
	{
		ele->QueryIntAttribute("id", &i);
	}
	pTBC->id = i + 1;
	tinyxml2::XMLElement *caseElement = myDocument->NewElement("case");
	TBCRoot->LinkEndChild(caseElement);
	caseElement->SetAttribute("id", pTBC->id);
	tinyxml2::XMLElement *injectElement = myDocument->NewElement("inject");
	tinyxml2::XMLElement *identiEle = myDocument->NewElement("identify");
	tinyxml2::XMLElement *injectPostElement = myDocument->NewElement("injectPost");
	tinyxml2::XMLElement *identiPostEle = myDocument->NewElement("identifyPost");
	caseElement->LinkEndChild(injectElement);
	caseElement->LinkEndChild(injectPostElement);
	caseElement->LinkEndChild(identiEle);
	caseElement->LinkEndChild(identiPostEle);

	tinyxml2::XMLText *injectText = myDocument->NewText(pTBC->inject.c_str());
	tinyxml2::XMLText *identiText = myDocument->NewText(pTBC->identify.c_str());
	tinyxml2::XMLText *injectPostText = myDocument->NewText(pTBC->injectPost.c_str());
	tinyxml2::XMLText *identiPostText = myDocument->NewText(pTBC->identifyPost.c_str());
	injectElement->LinkEndChild(injectText);
	identiEle->LinkEndChild(identiText);
	injectPostElement->LinkEndChild(injectPostText);
	identiPostEle->LinkEndChild(identiPostText);

	TBCRoot->QueryIntAttribute("num", &i);
	TBCRoot->SetAttribute("num", i + 1);
}

bool SQLiTest::test(CHttpClient *pHttpClient, Item *pItem)
{
	bool flag = false;
	long averageTime1 = 0;
	long averageTime2 = 0;

	static clock_t sqlTotalTime = 0;
	pHttpClient->setTotalTime(0);

	for (unsigned i = 0; i < pItem->getArgs().size(); i++)
	{
		if (((pItem->getArgs())[i].getSecurityFlag() & 1) != 0)	//这个参数肯定不用测试，如submit。
			continue;
		if (m_errorBased)
		{
			if (errorBasedTest(pHttpClient, pItem, i, averageTime1))
			{
				flag = true;
				continue;
			}
		}
		if (m_boolBased)
		{
			if (boolBasedTest(pHttpClient, pItem, i, averageTime2))
			{
				flag = true;
				continue;
			}
		}
		if (m_timeBased)
		{
			if (timeBasedTest(pHttpClient, pItem, i, (averageTime1 + averageTime2) / 2))
			{
				flag = true;
				continue;
			}
		}
	}
	sqlTotalTime += pHttpClient->getTotalTime();
	_cprintf("download sql usedTime:%d\t\t", sqlTotalTime);
	return flag;
}

bool SQLiTest::errorBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime)
{
	long sumTime = 0;
	int sendCount = 0;

	string html = "";
	Cookie cookie;
	string cookieStr;
	string args = "";
	TestResult* pResult = NULL;
	m_pData->getCookie(cookie);
	cookieStr = cookie.toString();
	HttpMethod method = pItem->getMethod();
	string url = pItem->getUrl();

	int resultState = 0;	//1 表示只有inject有，2表示两个都有。
	for (unsigned i = 0; i < m_vecEBTestCase.size(); i++)
	{
		resultState = 0;
		html = "";
		//参数编制。
		args = pItem->getArgsStr(pos, m_vecEBTestCase[i]->inject);

		send(pHttpClient, method, cookieStr, url, args, html, sumTime, sendCount);

		if (pHttpClient->getStatusCode() / 100 == 5)
		{
			//返回码为服务器内部错误，则认为有漏洞。
			resultState = 1500;
		}
		else if (pHttpClient->getStatusCode() / 100 == 4)
		{
			//	resultState = 1000 + pHttpClient->getStatusCode();
			continue;
		}
		else if (html.find(m_vecEBTestCase[i]->check) != -1)
		{
			resultState = 101;
		}

		//如果找到了报错，则不用验证。
		//html = "";
		//if (resultState == 1)
		//{
		//	args = pItem->getArgsStr(i, EBCvec[i]->identify);

		//	send(pHttpClient, method, cookieStr, url, args, html, sumTime);
		//	sendCount++;

		//	if (pHttpClient->getStatusCode() / 100 == 5){
		//		//返回码为服务器内部错误，则认为有漏洞。
		//		resultState++;
		//	}
		//	else if (html.find(EBCvec[i]->check) != -1)
		//	{
		//		resultState++;
		//	}
		//}

		if (resultState != 0)
		{
			//记录情况
			pResult = new TestResult();
			pResult->itemId = pItem->getId();
			pResult->url = pItem->getUrl();
			pResult->caseId = m_vecEBTestCase[i]->id;
			pResult->injectPos = pos;
			pResult->cookie = cookie.toString();
			pResult->args = pItem->getArgsStr(-1, "", true, false);	//原始参数
			pResult->resultState = resultState;
			pResult->argStrs = pItem->getArgsStr(pos, m_vecEBTestCase[i]->inject, true, false);
			pResult->type = 0;
			pResult->method = method;
			/*if (resultState == 101)
			{
			pResult->vecResponse.push_back(html);
			}*/
			m_pTestManager->putResultItem(pResult);
			//pItem->getArgs()[pos].setResultId(pResult->id);
			pItem->setResultId(pos, pResult->id);

			WriteFile("网址_结果格式化.txt", generateResult(pResult->id, pResult->resultState, pResult->url, pResult->method, pResult->args, pResult->argStrs, m_pTestManager->g_separator));
			break;
		}
	}
	//这里有一个小问题，如果sendCount=0，那么平均时间将无法计算。 可以使用两个参数，总时间与次数。当前仅使用平均时间。
	averageTime = sendCount == 0 ? 0 : sumTime * 1000 / CLOCKS_PER_SEC / sendCount;
	return resultState > 0;
}

bool SQLiTest::boolBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime)
{


	long sumTime = 0;
	int sendCount = 0;
	CURLcode code;
	bool isInjectable = false;
	string html = "";
	Cookie cookie;
	string cookieStr;
	string args = "";
	TestResult* pResult = NULL;
	m_pData->getCookie(cookie);
	int resultState = 0;
	cookieStr = cookie.toString();
	HttpMethod method = pItem->getMethod();
	string url = pItem->getUrl();
	unsigned int testCaseNum = m_vecBBTestCase.size() / 3;
	// and 1=1, or 1=1, and 1=2; 三种类型，每种类型数量都相同。

	string oriHtml;
	string errorHtml;
	string rightHtml;
	string rightIdentiHtml;
	string errorIdentiHtml;
	unsigned i;

	if ((pItem->getArgs())[pos].getValue() != "")	//是否有默认值
	{
		//该位置有默认值
		args = pItem->getArgsStr(-1, "", true, true);
		code = send(pHttpClient, method, cookieStr, url, args, oriHtml, sumTime, sendCount);	//获取样本0
		if (code == CURLE_OK)
		{
			switch (pHttpClient->getStatusCode() / 100)
			{
				case 2:
					for (i = 0; i < testCaseNum; i++)
					{
						rightHtml = "";
						errorHtml = "";

						args = pItem->getArgsStr(pos, m_vecBBTestCase[i]->inject) + getComment(method);
						code = send(pHttpClient, method, cookieStr, url, args, rightHtml, sumTime, sendCount);	//获取样本1
						if (code != CURLE_OK || pHttpClient->getStatusCode() / 100 != 2)
							continue;
						if (htmlEqual(oriHtml, rightHtml))
						{
							args = pItem->getArgsStr(pos, m_vecBBTestCase[testCaseNum * 2 + i]->inject) + getComment(method);
							code = send(pHttpClient, method, cookieStr, url, args, errorHtml, sumTime, sendCount);	//获取样本2
							if (code != CURLE_OK || pHttpClient->getStatusCode() / 100 != 2)
								continue;
							if (!htmlEqual(rightHtml, errorHtml))
							{
								resultState = 201;

								break;
							}
						}
					}
				default:// 未正常返回网页，表示参数有问题直接当做无参数处理
					break;
			}
		}
	}
	if (resultState == 0)
	{
		//无论有没有默认值，只有没有查出注入点就继续
		for (i = testCaseNum; i < testCaseNum * 2; i++)
		{
			rightHtml = "";
			errorHtml = "";
			rightIdentiHtml = "";
			errorIdentiHtml = "";
			args = pItem->getArgsStr(pos, m_vecBBTestCase[i]->inject, false) + getComment(method);
			code = send(pHttpClient, method, cookieStr, url, args, rightHtml, sumTime, sendCount);	//获取样本1
			if (code != CURLE_OK || pHttpClient->getStatusCode() / 100 != 2)
				continue;
			args = pItem->getArgsStr(pos, m_vecBBTestCase[testCaseNum + i]->inject, false) + getComment(method);
			code = send(pHttpClient, method, cookieStr, url, args, errorHtml, sumTime, sendCount);	//获取样本2
			if (!htmlEqual(rightHtml, errorHtml))
			{
				//进一步验证		//可以避免因为反射型网页误导。
				args = pItem->getArgsStr(pos, m_vecBBTestCase[i]->identify, false);
				code = send(pHttpClient, method, cookieStr, url, args, rightIdentiHtml, sumTime, sendCount);	//获取验证样本1
				if (code != CURLE_OK || pHttpClient->getStatusCode() / 100 != 2)
					continue;
				if (htmlEqual(rightHtml, rightIdentiHtml))
				{
					resultState = 203;
					args = pItem->getArgsStr(pos, m_vecBBTestCase[testCaseNum + i]->identify, false);
					send(pHttpClient, method, cookieStr, url, args, errorIdentiHtml, sumTime, sendCount);	//获取验证样本2
					if (htmlEqual(errorHtml, errorIdentiHtml))
					{
						resultState = 2031;
					}
					break;
				}
			}
		}
	}


	if (resultState > 0)
	{
		pResult = new TestResult();
		pResult->itemId = pItem->getId();
		pResult->url = pItem->getUrl();
		pResult->caseId = m_vecBBTestCase[i]->id;
		pResult->injectPos = pos;
		pResult->cookie = cookie.toString();
		pResult->args = pItem->getArgsStr(-1, "", true, false);
		pResult->resultState = resultState;
		pResult->type = 1;
		pResult->method = method;
		if (resultState == 201)
		{
			/*pResult->vecResponse.push_back(oriHtml);
			pResult->vecResponse.push_back(rightHtml);
			pResult->vecResponse.push_back(errorHtml);*/
			pResult->argStrs = pItem->getArgsStr(-1, "", true, false) + m_pTestManager->g_separator +
				pItem->getArgsStr(pos, m_vecBBTestCase[i]->inject, true, false) + m_pTestManager->g_separator +
				pItem->getArgsStr(pos, m_vecBBTestCase[testCaseNum * 2 + i]->inject, true, false);
		}
		else
		{
			/*pResult->vecResponse.push_back(rightHtml);
			pResult->vecResponse.push_back(rightIdentiHtml);
			pResult->vecResponse.push_back(errorHtml);
			pResult->vecResponse.push_back(errorIdentiHtml);*/
			pResult->argStrs = pItem->getArgsStr(pos, m_vecBBTestCase[i]->inject, false, false) + m_pTestManager->g_separator +
				pItem->getArgsStr(pos, m_vecBBTestCase[testCaseNum + i]->inject, false, false) + m_pTestManager->g_separator +
				pItem->getArgsStr(pos, m_vecBBTestCase[i]->identify, false, false) + m_pTestManager->g_separator +
				pItem->getArgsStr(pos, m_vecBBTestCase[testCaseNum + i]->identify, false, false);
		}
		m_pTestManager->putResultItem(pResult);
		pItem->setResultId(pos, pResult->id);
		WriteFile("网址_结果格式化.txt", generateResult(pResult->id, pResult->resultState, pResult->url, pResult->method, pResult->args, pResult->argStrs, m_pTestManager->g_separator));
	}

	averageTime = sendCount == 0 ? 0 : sumTime * 1000 / CLOCKS_PER_SEC / sendCount;
	return resultState > 0;
}

bool SQLiTest::timeBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long averageTime)
{
	clock_t start, end;
	string html = "";
	Cookie cookie;
	string args = "";
	TestResult* pResult = NULL;
	m_pData->getCookie(cookie);
	int resultState = 0;	//1 

	CURLcode code;
	pHttpClient->setTimeOut(averageTime + m_lateTime);
	//WriteLog("avergeTime" + to_string(averageTime) + "\t\turl:" + pItem->getUrl() + pItem->getArgsStr(-1, "", true, false));
	//code具有一定的延迟，为什么不知道(因为超时只是客户端超时，但是服务端仍然在执行sql语句，导致下一个请求发送时，服务端还在处理上一个请求)。
	//使用自己取得时间判断，没有用。服务端不会改变。
	for (unsigned i = 0; i < m_veerTBTestCase.size(); i++)
	{
		resultState = 0;
		args = pItem->getArgsStr(pos, m_veerTBTestCase[i]->inject + to_string(averageTime / 1000 + m_lateTime / 1000) + m_veerTBTestCase[i]->injectPost + getComment(pItem->getMethod()));

		start = clock();
		code = pHttpClient->send(pItem->getMethod(), cookie.toString(), pItem->getUrl(), args, html);
		end = clock();
		switch (code)
		{
			
			case CURLE_OPERATION_TIMEDOUT:
				//超时，说明if起作用了,即可插入
				resultState = 301;
				break;
			case CURLE_OK:
				if ((end - start) > (averageTime + m_lateTime))
				{
					//也说明延迟时间有问题。可能执行的sleep函数
					resultState=302;
				}
				break;
			default:
				WriteLog("timeBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long averageTime)----curlCode=" + to_string(code));
				break;
		}
		_cprintf("1:%d   %d\n", code, i);
		if (resultState > 0)
		{
			//进一步验证，不知道如何验证。正常情况下不超时。应该为curle_ok且时间小于超时时间。
			args = pItem->getArgsStr(pos, m_veerTBTestCase[i]->identify + getComment(pItem->getMethod()));

			start = clock();
			code = pHttpClient->send(pItem->getMethod(), cookie.toString(), pItem->getUrl(), args, html);
			end = clock();
			if ((code != CURLE_OK) || (end - start)> (averageTime + m_lateTime))
			{
				resultState = 0;
			}
			_cprintf("2:%d   %d\n", code, i);
		}

	//clock_t during;
	//for (unsigned i = 0; i < m_veerTBTestCase.size(); i++)
	//{
	//	resultState = 0;
	//	args = pItem->getArgsStr(pos, m_veerTBTestCase[i]->inject + to_string(averageTime / 1000 + m_lateTime / 1000) + m_veerTBTestCase[i]->injectPost + getComment(pItem->getMethod()));

	//	start = clock();
	//	code = pHttpClient->send(pItem->getMethod(), cookie.toString(), pItem->getUrl(), args, html);
	//	end = clock();
	//	during = end - start;
	//	_cprintf("1: %d\t%d\t%d\t%d\t%d\n", code, i, during, end, start);
	//	if (during >= (averageTime + m_lateTime))
	//	{
	//		//超时，说明if起作用了,即可插入
	//		resultState = 301;

	//		//进一步验证，不知道如何验证。正常情况下不超时。应该为curle_ok且时间小于超时时间。
	//		args = pItem->getArgsStr(pos, m_veerTBTestCase[i]->identify + getComment(pItem->getMethod()));

	//		start = clock();
	//		code = pHttpClient->send(pItem->getMethod(), cookie.toString(), pItem->getUrl(), args, html);
	//		end = clock();
	//		during = (end - start);
	//		if ( during >= (averageTime + m_lateTime))
	//		{
	//			resultState = 0;
	//		}
	//		_cprintf("2: %d\t%d\t%d\t%d\t%d\n", code, i, during, end, start);
	//	}

		if (resultState > 0)
		{
			pResult = new TestResult();
			pResult->itemId = pItem->getId();
			pResult->url = pItem->getUrl();
			pResult->caseId = m_vecBBTestCase[i]->id;
			pResult->injectPos = pos;
			pResult->cookie = cookie.toString();
			pResult->args = pItem->getArgsStr(-1, "", true, false);
			pResult->resultState = resultState;
			pResult->type = 2;
			pResult->method = pItem->getMethod();
			pResult->argStrs = pItem->getArgsStr(pos, m_veerTBTestCase[i]->inject + to_string(averageTime / 1000 + 1) + m_veerTBTestCase[i]->injectPost + getComment(pItem->getMethod()), true, false);
			m_pTestManager->putResultItem(pResult);
			pItem->setResultId(pos, pResult->id);
			WriteFile("网址_结果格式化.txt", generateResult(pResult->id, pResult->resultState, pResult->url, pResult->method, pResult->args, pResult->argStrs, m_pTestManager->g_separator));
			break;
		}
	}
	pHttpClient->setTimeOut(-1);		//恢复默认的超时时间
	return resultState > 0;
}

BOOL SQLiTest::htmlEqual(string html, string html2)
{
	if (html.length() == html2.length())
		return true;
	return false;
}

string SQLiTest::getComment(HttpMethod method)
{
	string arg = "";
	if (method == HttpMethod::get)
	{
		//arg = "%20%23";
	}
	else if (method == HttpMethod::post)
	{
		//arg = "%20%23";
	}
	return arg;
}

CURLcode SQLiTest::send(CHttpClient*pHttpClient, HttpMethod method, string cookieStr, string url, string args, string& html, long&sumTime, int&sendCount)
{
	CURLcode code;
	static clock_t start, end;
	start = clock();
	code = pHttpClient->send(method, cookieStr, url, args, html);
	if (pHttpClient->getStatusCode() / 100 == 4)
	{
		html = "";
	}
	end = clock();
	sumTime += (end - start);
	sendCount++;
	return code;
}

void SQLiTest::setTestMode(bool errorBased, bool boolBased, bool timeBased)
{
	m_errorBased = errorBased;
	m_boolBased = boolBased;
	m_timeBased = timeBased;
}







