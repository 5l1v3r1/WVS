#include "stdafx.h"
#include "SQLiTest.h"
#include "tinyxml2.h"


CSQLiTest::CSQLiTest(CData* pData)
{
	EBCvec = vector<ErrorBasedCase*>();
	TBCvec = vector<TimeBasedCase*>();
	BBCvec = vector<BoolBasedCase*>();
	resultVec = vector<SQLiResult*>();
	m_pData = pData;
	loadConfiguration();
}


CSQLiTest::~CSQLiTest()
{

}

BOOL CSQLiTest::loadConfiguration(string fileName)
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
				pEBC->inject = " ";
			}
			else{
				pEBC->inject = injectElement->GetText();
			}

			pEBC->identify = identiEle->FirstChild()->Value();
			pEBC->check = checkElement->FirstChild()->Value();
			_cprintf("id:%d\tinject:%s\tidentify:%s\tcheck:%s\n", pEBC->id, pEBC->inject.c_str(), pEBC->identify.c_str(), pEBC->check.c_str());
			EBCvec.push_back(pEBC);
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
			BBCvec.push_back(pBBC);
			caseElement = caseElement->NextSiblingElement();
		}

		tinyxml2::XMLElement *TBCRoot = BBCRoot->NextSiblingElement();

		num = 0;
		TBCRoot->QueryIntAttribute("num", &num);
		PTimeBasedCase pTBC;
		caseElement = TBCRoot->FirstChildElement();
		for (int i = 0; i < num; i++){
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
			TBCvec.push_back(pTBC);
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



BOOL CSQLiTest::saveConfiguration(string fileName /*= "test.xml"*/)
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
		bbcCase = { 0, "2 and 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "2) and 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);

		bbcCase = { 0, "' or 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\" or 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "') or 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\") or 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "2 or 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "2) or 1=1 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);

		bbcCase = { 0, "' and 1=2 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\" and 1=2 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "') and 1=2 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "\") and 1=2 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "2 and 1=2 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);
		bbcCase = { 0, "2) and 1=2 #", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot, myDocument);



		TimeBasedCase tbcCase = { 0, "' or if(1=1, sleep(", "), 0) #", "NULL", "NULL", 0 };
		insertTBC(&tbcCase, TBCRoot, myDocument);
		TimeBasedCase tbcCase2 = { 0, "\" or if(1=1, sleep(", "), 0) #", "NULL", "NULL", 0 };
		insertTBC(&tbcCase2, TBCRoot, myDocument);
		TimeBasedCase tbcCase3 = { 0, "') or if(1=1, sleep(", "), 0) #", "NULL", "NULL", 0 };
		insertTBC(&tbcCase, TBCRoot, myDocument);
		TimeBasedCase tbcCase4 = { 0, "\") or if(1=1, sleep(", "), 0) #", "NULL", "NULL", 0 };
		insertTBC(&tbcCase2, TBCRoot, myDocument);
		TimeBasedCase tbcCase5 = { 0, "23 or if(1=1, sleep(", "), 0) #", "NULL", "NULL", 0 };
		insertTBC(&tbcCase, TBCRoot, myDocument);
		TimeBasedCase tbcCase6 = { 0, "23) or if(1=1, sleep(", "), 0) #", "NULL", "NULL", 0 };
		insertTBC(&tbcCase2, TBCRoot, myDocument);

		myDocument->SaveFile(fileName.c_str());//保存到文件
	}
	catch (string& e)
	{
		WriteLog("SQLiTest SaveConfiguration failed" + e);
		return false;
	}
	return true;
}
void CSQLiTest::insertEBC(ErrorBasedCase *pEBC, tinyxml2::XMLElement *EBCRoot, tinyxml2::XMLDocument* myDocument)
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
void CSQLiTest::insertBBC(BoolBasedCase *pBBC, tinyxml2::XMLElement *BBCRoot, tinyxml2::XMLDocument* myDocument)
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
void CSQLiTest::insertTBC(TimeBasedCase *pTBC, tinyxml2::XMLElement *TBCRoot, tinyxml2::XMLDocument* myDocument)
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

bool CSQLiTest::test(CHttpClient *pHttpClient, Item *pItem)
{

	long averageTime1 = 0;
	long averageTime2 = 0;

	for (unsigned i = 0; i < pItem->getArgs().size(); i++)
	{
		if (m_errorBased)
		{
			if (errorBasedTest(pHttpClient, pItem, i, averageTime1))
			{
				continue;
			}
		}
		if (m_boolBased)
		{
			if (boolBasedTest(pHttpClient, pItem, i, averageTime2))
			{
				continue;
			}
		}
		if (m_timeBased)
		{
			if (timeBasedTest(pHttpClient, pItem, i, (averageTime1 + averageTime2) / 2))
			{
				continue;
			}
		}
	}
	return false;
}

bool CSQLiTest::errorBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime)
{
	long sumTime = 0;
	int sendCount = 0;

	string html = "";
	Cookie cookie;
	string cookieStr;
	string args = "";
	SQLiResult* pResult = NULL;
	m_pData->getCookie(cookie);
	cookieStr = cookie.toString();
	HttpMethod method = pItem->getMethod();
	string url = pItem->getUrl();

	int resultState = 0;	//1 表示只有inject有，2表示两个都有。
	for (unsigned i = 0; i < EBCvec.size(); i++)
	{
		resultState = 0;
		html = "";
		//参数编制。
		args = pItem->getArgsStr(pos, EBCvec[i]->inject);

		send(pHttpClient, method, cookieStr, url, args, html, sumTime, sendCount);

		if (pHttpClient->getStatusCode() / 100 == 5){
			//返回码为服务器内部错误，则认为有漏洞。
			resultState++;
		}
		else if (html.find(EBCvec[i]->check) != -1)
		{
			resultState++;
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
			pResult = new SQLiResult();
			pResult->id = resultVec.size();
			pResult->itemId = pItem->getId();
			pResult->url = pItem->getUrl();
			pResult->caseId = EBCvec[i]->id;
			pResult->injectPos = pos;
			pResult->cookie = cookie.toString();
			pResult->args = pItem->getArgsStr();
			pResult->resultState = resultState;
			pResult->type = 0;
			resultVec.push_back(pResult);
			break;
		}
	}
	averageTime = sumTime / (sendCount * 1000 / CLOCKS_PER_SEC);

	return resultState > 0;
}

bool CSQLiTest::boolBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long &averageTime)
{


	long sumTime = 0;
	int sendCount = 0;

	bool isInjectable = false;
	string html = "";
	Cookie cookie;
	string cookieStr;
	string args = "";
	SQLiResult* pResult = NULL;
	m_pData->getCookie(cookie);
	int resultState = 0;
	cookieStr = cookie.toString();
	HttpMethod method = pItem->getMethod();
	string url = pItem->getUrl();
	unsigned int testCaseNum = BBCvec.size() / 3;
	// and 1=1, or 1=1, and 1=2; 三种类型，每种类型数量都相同。

	if (url.find("blind") != -1)
	{
		//int x = 1;
	}

	string oriHtml;
	string errorHtml;
	string rightHtml;
	unsigned i;

	if ((pItem->getArgs())[pos].getValue() != "")	//是否有默认值
	{
		//该位置有默认值
		args = pItem->getArgsStr();
		send(pHttpClient, method, cookieStr, url, args, oriHtml, sumTime, sendCount);	//获取样本0
		for (i = 0; i < testCaseNum; i++)
		{
			rightHtml = "";
			errorHtml = "";

			args = pItem->getArgsStr(pos, BBCvec[i]->inject) + getComment(method);
			send(pHttpClient, method, cookieStr, url, args, rightHtml, sumTime, sendCount);	//获取样本1
			if (htmlEqual(oriHtml, rightHtml))
			{
				args = pItem->getArgsStr(pos, BBCvec[testCaseNum * 2 + i]->inject) + getComment(method);
				send(pHttpClient, method, cookieStr, url, args, errorHtml, sumTime, sendCount);	//获取样本2
				if (!htmlEqual(rightHtml, errorHtml))
				{
					resultState = 101;
					break;
				}
			}
		}
		if (i == testCaseNum)
		{
			//有默认值，但是是错误的。从 or 1=1类型开始测试
			for (; i < testCaseNum * 2; i++)
			{
				rightHtml = "";

				args = pItem->getArgsStr(pos, BBCvec[i]->inject) + getComment(method);
				send(pHttpClient, method, cookieStr, url, args, rightHtml, sumTime, sendCount);	//获取样本1
				if (!htmlEqual(oriHtml, rightHtml))
				{
					resultState = 102;
					break;
				}
			}
		}
	}
	else
	{
		for (i = testCaseNum; i < testCaseNum * 2; i++)
		{
			rightHtml = "";
			errorHtml = "";
			args = pItem->getArgsStr(pos, BBCvec[i]->inject) + getComment(method);
			send(pHttpClient, method, cookieStr, url, args, rightHtml, sumTime, sendCount);	//获取样本1

			args = pItem->getArgsStr(pos, BBCvec[testCaseNum + i]->inject) + getComment(method);
			send(pHttpClient, method, cookieStr, url, args, errorHtml, sumTime, sendCount);	//获取样本2
			if (!htmlEqual(rightHtml, errorHtml))
			{
				resultState =103;
				break;
			}
		}
	}

	if (resultState > 0)
	{
		//进一步验证   用基于时间的测试来验证。
		
	}
	if (resultState > 0)
	{
		pResult = new SQLiResult();
		pResult->id = resultVec.size();
		pResult->itemId = pItem->getId();
		pResult->url = pItem->getUrl();
		pResult->caseId = BBCvec[i]->id;
		pResult->injectPos = pos;
		pResult->cookie = cookie.toString();
		pResult->args = pItem->getArgsStr();
		pResult->resultState = resultState;
		pResult->type = 1;

		resultVec.push_back(pResult);
	}

	averageTime = sumTime / (sendCount * 1000 / CLOCKS_PER_SEC);
	return resultState > 0;
}

bool CSQLiTest::timeBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long averageTime)
{
	clock_t start, end;
	string html = "";
	Cookie cookie;
	string args = "";
	SQLiResult* pResult = NULL;
	m_pData->getCookie(cookie);
	int resultState = 0;	//1 

	CURLcode code;
	pHttpClient->setTimeOut(averageTime + m_lateTime);
	for (unsigned i = 0; i < TBCvec.size(); i++)
	{
		resultState = 0;
		args = pItem->getArgsStr(pos, TBCvec[i]->inject + to_string(averageTime / 1000 + 1) + TBCvec[i]->injectPost + getComment(pItem->getMethod()));


		start = clock();
		code = pHttpClient->send(pItem->getMethod(), cookie.toString(), pItem->getUrl(), args, html);
		end = clock();
		switch (code)
		{
			case CURLE_OPERATION_TIMEDOUT:
				//超时，说明if起作用了,即可插入
				resultState++;
				break;
			case CURLE_OK:
				if ((end - start) > averageTime + m_lateTime)
				{
					//也说明延迟时间有问题。可能执行的sleep函数
					resultState++;
				}
				break;
			default:
				WriteLog("timeBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long averageTime)----curlCode=" + to_string(code));
				break;
		}
		if (resultState == 1)
		{
			//进一步验证，不知道如何验证。
		}
		if (resultState > 0)
		{
			pResult = new SQLiResult();
			pResult->id = resultVec.size();
			pResult->itemId = pItem->getId();
			pResult->url = pItem->getUrl();
			pResult->caseId = BBCvec[i]->id;
			pResult->injectPos = pos;
			pResult->cookie = cookie.toString();
			pResult->args = pItem->getArgsStr();
			pResult->resultState = resultState;
			pResult->type = 2;
			resultVec.push_back(pResult);
			break;
		}
	}
	pHttpClient->setTimeOut(0);
	return resultState > 0;
}




std::string CSQLiTest::resultToString()
{
	string args = "\n\n----------------showResult(resultId, type, resultState, caseId, url, args,injectPos)-------------------------------\n";
	for (unsigned i = 0; i < resultVec.size(); i++)
	{
		args += to_string(resultVec[i]->id) +
			"\t" + to_string(resultVec[i]->type) +
			"\t" + to_string(resultVec[i]->resultState) +
			"\t" + to_string(resultVec[i]->caseId) +
			"\t" + resultVec[i]->url +
			"\t" + resultVec[i]->args +
			"\t" + to_string(resultVec[i]->injectPos) +
			"\n-----------------------------------------------------------\n\n";
	}
	return args;
}
std::string CSQLiTest::resultToStringForCSV()
{
	string args = "resultId, type, resultState, caseId, url,injectPos, args,cookie\n";
	for (unsigned i = 0; i < resultVec.size(); i++)
	{
		args += to_string(resultVec[i]->id) +
			"," + to_string(resultVec[i]->type) +
			"," + to_string(resultVec[i]->resultState) +
			"," + to_string(resultVec[i]->caseId) +
			"," + resultVec[i]->url +
			"," + to_string(resultVec[i]->injectPos)+
			"," + resultVec[i]->args +
			"," + resultVec[i]->cookie +
			
			"\n";
	}
	return args;
}

BOOL CSQLiTest::htmlEqual(string html, string html2)
{
	if (html == html2)
		return true;
	return false;
}

string CSQLiTest::getComment(HttpMethod method)
{
	string arg="";
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

CURLcode CSQLiTest::send(CHttpClient*pHttpClient, HttpMethod method, string cookieStr, string url, string args, string& html, long&sumTime, int&sendCount)
{
	CURLcode code;
	static clock_t start, end;
	start = clock();
	code = pHttpClient->send(method, cookieStr, url, args, html);
	end = clock();
	sumTime += (end - start);
	sendCount++;
	return code;
}

void CSQLiTest::setTestMode(bool errorBased, bool boolBased, bool timeBased)
{
	m_errorBased = errorBased;
	m_boolBased = boolBased;
	m_timeBased = timeBased;
}

void CSQLiTest::clearResult()
{
	resultVec.clear();
}





