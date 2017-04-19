#include "stdafx.h"
#include "Data.h"

CData::CData()
{
	originUrl = "";
	m_itemMutex = CThreadMutex();
}

CData::~CData()
{

}

//************************************
// Returns:   bool   存在过：true;    未出现过false；
// Parameter: Item & des :
// Parameter: vector<Item> & crawlerLinksVec :
// Function:  查看是否已经存在过。
//************************************
bool CData::checkInLinks(Item &des, vector<Item*>&crawlerLinksVec)
{
	if (des.getUrl().find(domain) == -1)
	{
		cout << "不属于本网站的网址！" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().size() >= 5 && (des.getUrl().c_str()[4] == 's' || des.getUrl().c_str()[4] == 'S'))
	{
		cout << "该网址为https,暂不支持!" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().find("logout") != -1)
	{
		cout << "退出登陆网址，不要" << des.getUrl() << endl;
		return true;
	}
	m_itemMutex.Lock();
	for (unsigned int i = 0; i < crawlerLinksItemVec.size(); i++)
	{
		if (des.equal(*(crawlerLinksItemVec[i])))
		{
			cout << "这个链接已经存在过！" << des.getUrl() << endl;
			return true;
		}
	}
	m_itemMutex.Unlock();
	cout << "新链接！" << des.getUrl() << endl;
	return false;
}

string CData::vecFieldToString(vector<Field> fieldVec)
{
	string args;
	for (unsigned int i = 0; i < fieldVec.size(); i++)
	{
		args += fieldVec[i].toString();
		if (i != fieldVec.size() - 1)
		{
			args += "&";
		}
	}
	return args;
}


void CData::showCrawler()
{
	m_itemMutex.Lock();
	cout << "-------------------------------------------------------------------" << endl;
	cout << "url:" << crawlerLinksItemVec[crawledNum]->getUrl().c_str() << endl;
	cout << "cookie:" << cookie.toString() << endl;
	cout << "method:" << crawlerLinksItemVec[crawledNum]->getMethod() << endl;
	cout << "args:" << vecFieldToString(crawlerLinksItemVec[crawledNum]->getArgs()) << endl;
	cout << crawledNum << endl;
	cout << "-------------------------------------------------------------------" << endl << endl;
	WriteFile("网址树.txt", crawlerLinksItemVec[crawledNum]->getUrl() + string("\tMethod:") + (char)(crawlerLinksItemVec[crawledNum]->getMethod() + '0') + string("\tAEGUMENT:") + vecFieldToString(crawlerLinksItemVec[crawledNum]->getArgs()));
	m_itemMutex.Unlock();
}


void CData::analyseHeader(string& strHeader)
{
	Cookie newCookie(strHeader);
	if (!(cookie == newCookie))
	{
		//cookie = newCookie;
		cookie = newCookie;
		_cprintf("cookie:%s\n", newCookie.toString().c_str());
	}
	strHeader = "";
}

vector<Item*>* CData::analyseHtml(Item*pItem, string& strHtml)
{
	vector<Item*> *pItemVec = new vector<Item*>();
	vector<string>linksVec;	//暂存 从一个网页中提取的多个links，
	vector<string>baseVec;	//暂存 从一个网页中提取的base
	vector<string>formStrVec;//暂存  form
	vector<HtmlForm>formVec; //暂存 
	//1. 获取baseUrl。 首先寻找base标签，如果没有则获取该网页地址去除最后一段用‘\’分隔开的字符串作为baseurl。
	findByRegex(strHtml, baseRegex, baseVec, false);
	string baseUrl;
	if (baseVec.size() > 1)
	{
		string FileName = "Base" + pItem->getUrl();
		WriteLog("多于一个base标签:" + vecStrToString(baseVec) + "\n原文见文件:" + FileName + "\n");
		baseUrl = pItem->getUrl();
	}
	else if (baseVec.size() == 1)
	{
		if (!findByName(baseVec[0], "href", baseUrl, false))
		{
			WriteLog("BaseUrl提取失败: baseTag = " + baseVec[0] + "提取内容:" + baseUrl.c_str() + "\n");
			baseUrl = pItem->getUrl();
		}
	}
	else
	{
		baseUrl = pItem->getUrl();
	}
	_cprintf("baseUrl:%s\n", baseUrl.c_str());
	int posOfEnd = baseUrl.rfind('/');
	if (posOfEnd < 6)
	{
		WriteLog("Url 太短,里面没有/，只有协议（http://）:" + pItem->getUrl() + "\n");
		exit(1);
	}
	else	
	{
		baseUrl = baseUrl.substr(0, posOfEnd + 1);
	}

	//2. 获取网页内所有的<a>标签，提取其中的href字段，格式化为Item,设置请求方式为get。去重比较，若不重复则加入待扫描队列。
	findByRegex(strHtml, linksRegex, linksVec, false);
	string tempLink;
	Item *pTempNewItem;
	vector<Field>*pArgs;
	string argStr;
	for (unsigned int i = 0; i < linksVec.size(); i++)
	{
		pTempNewItem = new Item();
		argStr = "";
		findByName(linksVec[i], "href", tempLink, false);
		formatLink(baseUrl, tempLink, argStr);
		pTempNewItem->setUrl(tempLink);
		if ((pArgs = getAgrs(argStr)) != NULL)
		{
			pTempNewItem->setArgs(*pArgs);
			delete pArgs;
		}
		pTempNewItem->setMethod(HttpMethod::get);

		if (!checkInLinks(*pTempNewItem, crawlerLinksItemVec))
		{
			pTempNewItem->setLayer(pItem->getLayer() + 1);
		//	crawlerLinksItemVec.push_back(*pTempNewItem);	//mutex area;
		//	CExtractJob *job = new CExtractJob(pTempNewItem, this);
		//	pThreadManage->Run(job, NULL);
		//	pItemVec->push_back(pTempNewItem);
			putItem(pTempNewItem);
		}
		else
		{
			//	cout << "这个链接已经存在过！" << pTempNewItem->getUrl() << endl;
			delete pTempNewItem;
		}
	}

	//3. 获取所有的form表单。对每个表单中的input标签进行提取并格式化为Field。最后将每个表单存储为Item,设置请求方式为post。去重比较，若不重复则加入待扫描队列。
	findByRegex(strHtml, HtmlForm::FORM_REGEX, formStrVec, false);
	HtmlForm* form;
	for (unsigned int i = 0; i < formStrVec.size(); i++)
	{
		form = new HtmlForm(formStrVec[i]);
		cout << form->toString().c_str() << endl;
		formVec.push_back(*form);
	}
	for (unsigned int i = 0; i < formVec.size(); i++)
	{
		tempLink = formVec[i].getAction();
		pTempNewItem = new Item();
		formatLink(baseUrl, tempLink, argStr);
		pTempNewItem->setUrl(tempLink);
		pTempNewItem->setMethod(HttpMethod::post);
		pTempNewItem->setArgs(formVec[i].getFields());
		if (!checkInLinks(*pTempNewItem, crawlerLinksItemVec))
		{
			pTempNewItem->setLayer(pItem->getLayer() + 1);
		//	crawlerLinksItemVec.push_back(*pTempNewItem);	//mutex area
		//	CExtractJob *job = new CExtractJob(pTempNewItem, this);
		//	pThreadManage->Run(job, NULL);
		//	pItemVec->push_back(pTempNewItem);
			putItem(pTempNewItem);
		}
		else
		{
			//	cout << "这个链接已经存在过！" << pTempNewItem->getUrl() << endl;
			delete pTempNewItem;
		}
	}
	linksVec.clear();
	formVec.clear();
	baseVec.clear();
	formStrVec.clear();
	return pItemVec;
}

Item* CData::getItem()
{
	Item * pItme;
	m_itemMutex.Lock();
	if (crawlerLinksItemVec.size() <= crawledNum)
	{
		pItme = NULL;
	}
	else{
		pItme = crawlerLinksItemVec[crawledNum];
		crawledNum++;
	}
	m_itemMutex.Unlock();
	return pItme;
}

void CData::putItem(Item* pItem)
{
	m_itemMutex.Lock();
	crawlerLinksItemVec.push_back(pItem);
	m_itemMutex.Unlock();
}


