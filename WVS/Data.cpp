#include "stdafx.h"
#include "Data.h"

void CData::initData()
{
	InitializeSRWLock(&m_linksVecSRW);
	InitializeSRWLock(&m_cookieSRW);
	InitializeSRWLock(&m_extractLinkNumSRW);
	InitializeSRWLock(&m_testedArgNumSRW);
}

CData::CData(string oriUrl)
{
	setUrl(oriUrl);
	initData();
}

CData::CData()
{
	initData();
}
void CData::setUrl(string oriUrl)
{
	if (oriUrl.find("http") != 0)
	{
		oriUrl = "http://" + oriUrl;
	}
	originUrl = oriUrl;
	int posBeg = oriUrl.find('/');
	int posEnd = (oriUrl.substr(posBeg + 2, oriUrl.size())).find('/');
	if (posEnd == -1){
		//		http://www.baidu.com
		domain = oriUrl.substr(posBeg + 2, oriUrl.size());
	}
	else{
		domain = oriUrl.substr(posBeg + 2, posEnd);
	}
	hasSetUrl = true;
}



CData::~CData()
{

}

//************************************
// Returns:   bool   ���ڹ���true;    δ���ֹ�false��
// Parameter: Item & des :
// Parameter: vector<Item> & crawlerLinksVec :
// Function:  �鿴�Ƿ��Ѿ����ڹ���
//************************************
bool CData::checkInLinks(Item &des, vector<Item*>&crawlerLinksVec)
{
	crawledNum = 100;
	if (des.getUrl().find(domain) == -1)
	{
	//	cout << "�����ڱ���վ����ַ��" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().size() >= 5 && (des.getUrl().c_str()[4] == 's' || des.getUrl().c_str()[4] == 'S'))
	{
	//	cout << "����ַΪhttps,�ݲ�֧��!" << des.getUrl() << endl;
		return true;
	}
	//else if (des.getUrl().find("logout") != -1)
	//{
	//	//cout << "�˳���½��ַ����Ҫ" << des.getUrl() << endl;
	//	return true;
	//}
	////else if (des.getUrl().find("captcha") != -1)
	////{
	////	//cout << "��Ҫһ����ǽ����֤�룬�ᵼ�������ٶȾ������Ҫ" << des.getUrl() << endl;
	////	return true;
	////}
	//else if (des.getUrl().find("setup-db.php") != -1)
	//{
	//	//cout << "sqli���ݿ����ã���Ҫ" << des.getUrl() << endl;
	//	return true;
	//}
	//else if (des.getUrl().find("setup.php") != -1)
	//{
	//	//cout << "dvwa���ݿ����ã���Ҫ" << des.getUrl() << endl;
	//	return true;
	//}
	//else if (des.getUrl().find("csrf") != -1)
	//{
	//	//cout << "�������ã���Ҫ" << des.getUrl() << endl;
	//	return true;
	//}
	/*if (test == false)
	{
		for (unsigned int i = 0; i < excludeUrl.size(); i++)
		{
			
				cout << "excludeѡ��Ϊ excludeurl:" << excludeUrl[i] << endl;
		}
		test = true;
	}*/
	for (unsigned int i = 0; i < excludeUrl.size(); i++)
	{
		if (des.getUrl().find(excludeUrl[i]) != -1)
		{
			//cout << "��excludeѡ��  url:" << des.getUrl() << "\texcludeurl:" << excludeUrl[i] << endl;
			return true;
		}
	}

	AcquireSRWLockShared(&m_linksVecSRW);
	for (unsigned int i = 0; i < crawlerLinksItemVec.size(); i++)
	{
		if (des.equal(*(crawlerLinksItemVec[i])))
		{
	//		cout << "��������Ѿ����ڹ���" << des.getUrl() << endl;
			ReleaseSRWLockShared(&m_linksVecSRW);
			return TRUE;
		}
	}
	ReleaseSRWLockShared(&m_linksVecSRW);
//	cout << "�����ӣ�" << des.getUrl() << endl;
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


//void CData::showCrawler()
//{
//	AcquireSRWLockShared(&m_linksVecSRW);
//	cout << "-------------------------------------------------------------------" << endl;
//	cout << "url:" << crawlerLinksItemVec[crawledNum]->getUrl().c_str() << endl;
//	cout << "cookie:" << cookie.toString() << endl;
//	cout << "method:" << crawlerLinksItemVec[crawledNum]->getMethod() << endl;
//	cout << "args:" << vecFieldToString(crawlerLinksItemVec[crawledNum]->getArgs()) << endl;
//	cout << crawledNum << endl;
//	cout << "-------------------------------------------------------------------" << endl << endl;
//	WriteFile("��ַ��.txt", crawlerLinksItemVec[crawledNum]->getUrl() + string("\tMethod:") + (char)(crawlerLinksItemVec[crawledNum]->getMethod() + '0') + string("\tAEGUMENT:") + vecFieldToString(crawlerLinksItemVec[crawledNum]->getArgs()));
//	ReleaseSRWLockShared(&m_linksVecSRW);
//}


void CData::analyseHeader(string& strHeader)
{
	Cookie newCookie(strHeader);
	if (newCookie.toString() == "")
	{
		//����cookieΪ��ʱ��������ȫ��cookie;
		strHeader = "";
		return;	
	}
	AcquireSRWLockExclusive(&m_cookieSRW);
	if (!(cookie == newCookie) && cookie.toString() == "")
	{
		cookie = newCookie;
	//	_cprintf("cookie:%s\n", newCookie.toString().c_str());
	}
	ReleaseSRWLockExclusive(&m_cookieSRW);
	strHeader = "";
}

vector<Item*>* CData::analyseHtml(Item*pItem, string& strHtml)
{
	string baseUrl = getBaseUrl(strHtml, pItem);
	vector<Item*> *pItemVec = new vector<Item*>();
	vector<string>linksVec;		//�ݴ� ��һ����ҳ����ȡ�Ķ��links��
	vector<string>formStrVec;	//�ݴ�  form
	vector<HtmlForm>formVec;	//�ݴ� 
	//1. ��ȡbaseUrl�� ����Ѱ��base��ǩ�����û�����ȡ����ҳ��ַȥ�����һ���á�\���ָ������ַ�����Ϊbaseurl��
	

	//2. ��ȡ��ҳ�����е�<a>��ǩ����ȡ���е�href�ֶΣ���ʽ��ΪItem,��������ʽΪget��ȥ�رȽϣ������ظ�������ɨ����С�
	findByRegex(strHtml, linksRegex, linksVec, false);
	string tempLink;
	Item *pTempNewItem;
	vector<Field>*pArgs;
	string argStr;
	pTempNewItem = new Item();
	for (unsigned int i = 0; i < linksVec.size(); i++)
	{
		
		argStr = "";
		pTempNewItem->reset();
		findByName(linksVec[i], "href", tempLink, false);
		formatLink(baseUrl, tempLink, argStr);
		pTempNewItem->setUrl(tempLink);
		pTempNewItem->setHash(0);
		if ((pArgs = getAgrs(argStr)) != NULL)
		{
			pTempNewItem->setArgs(*pArgs);
			delete pArgs;
		}
		pTempNewItem->setMethod(HttpMethod::get);

		if (!checkInLinks(*pTempNewItem, crawlerLinksItemVec))
		{
			pTempNewItem->setLayer(pItem->getLayer() + 1);
			pTempNewItem->setOriId(pItem->getId());
			pItemVec->push_back(pTempNewItem);
			putItem(pTempNewItem);
			pTempNewItem = new Item();
		}
	}

	//3. ��ȡ���е�form������ÿ�����е�input��ǩ������ȡ����ʽ��ΪField�����ÿ�����洢ΪItem,��������ʽΪpost��ȥ�رȽϣ������ظ�������ɨ����С�
	findByRegex(strHtml, HtmlForm::FORM_REGEX, formStrVec, false);
	HtmlForm* form;
	
	for (unsigned int i = 0; i < formStrVec.size(); i++)
	{
		
		form = new HtmlForm(formStrVec[i]);
		formVec.push_back(*form);
	}
	for (unsigned int i = 0; i < formVec.size(); i++)
	{
		string temp = formVec[i].getAction();
		tempLink = temp;
		pTempNewItem = new Item();
		formatLink(baseUrl, tempLink, argStr);
		pTempNewItem->setUrl(tempLink);
		pTempNewItem->setMethod(formVec[i].getMethod());
		pTempNewItem->setArgs(formVec[i].getFields());
		if (!checkInLinks(*pTempNewItem, crawlerLinksItemVec))
		{
			pTempNewItem->setLayer(pItem->getLayer() + 1);
			pTempNewItem->setOriId(pItem->getId());
			pItemVec->push_back(pTempNewItem);
			putItem(pTempNewItem);
		}
		else
		{
			//	cout << "��������Ѿ����ڹ���" << pTempNewItem->getUrl() << endl;
			delete pTempNewItem;
		}
	}
	linksVec.clear();
	formVec.clear();
	formStrVec.clear();
	return pItemVec;
}

Item* CData::analyseRedirectHeader(Item* pItem, string headerStr)
{
	Item *tempItem = NULL;
	regex e("Location: (.*)");
	smatch m;
	regex_search(headerStr, m, e);
	if (m.size() > 1)
	{
		string link=m[1];
		string args;
		string baseUrl = getBaseUrl("", pItem);
		formatLink(baseUrl, link, args);
		tempItem = new Item(HttpMethod::get, link);
		if (!checkInLinks(*tempItem, crawlerLinksItemVec))
		{
			tempItem->setLayer(pItem->getLayer());
			tempItem->setOriId(pItem->getId());
			putItem(tempItem);
		}
		else{
			tempItem = NULL;
		}
	}
	return tempItem;
}

//Item* CData::getItem()
//{
//	Item * pItme;
//	AcquireSRWLockExclusive(&m_linksVecSRW);
//	if (crawlerLinksItemVec.size() <= crawledNum)
//	{
//		pItme = NULL;
//	}
//	else
//	{
//		pItme = crawlerLinksItemVec[crawledNum];
//		crawledNum++;
//	}
//	ReleaseSRWLockExclusive(&m_linksVecSRW);
//	return pItme;
//}

Item* CData::getItemByIndex(unsigned index)
{
	Item * pItme;
	AcquireSRWLockShared(&m_linksVecSRW);
	if (crawlerLinksItemVec.size() <= index)
	{
		pItme = NULL;
	}
	else
	{
		pItme = crawlerLinksItemVec[index];
	}
	ReleaseSRWLockShared(&m_linksVecSRW);
	return pItme;
}



void CData::putItem(Item* pItem)
{
	AcquireSRWLockExclusive(&m_linksVecSRW);
	pItem->setId(crawlerLinksItemVec.size());
	crawlerLinksItemVec.push_back(pItem);
	m_totalArgNum += pItem->getArgs().size();
	ReleaseSRWLockExclusive(&m_linksVecSRW);
}

void CData::getCookie(Cookie& tempCookie)
{
	AcquireSRWLockShared(&m_cookieSRW);
	tempCookie = cookie;
	ReleaseSRWLockShared(&m_cookieSRW);
}

std::string CData::getBaseUrl(string strHtml, Item *pItem)
{
	vector<string>baseVec;		//�ݴ� ��һ����ҳ����ȡ��base
	findByRegex(strHtml, baseRegex, baseVec, false);
	string baseUrl;
	if (baseVec.size() > 1)
	{
		string FileName = "Base" + pItem->getUrl();
		WriteLog("����һ��base��ǩ:" + vecStrToString(baseVec) + "\nԭ�ļ��ļ�:" + FileName + "\n");
		baseUrl = pItem->getUrl();
	}
	else if (baseVec.size() == 1)
	{
		if (!findByName(baseVec[0], "href", baseUrl, false))
		{
			WriteLog("BaseUrl��ȡʧ��: baseTag = " + baseVec[0] + "��ȡ����:" + baseUrl.c_str() + "\n");
			baseUrl = pItem->getUrl();
		}
	}
	else
	{
		baseUrl = pItem->getUrl();
	}
	//_cprintf("baseUrl:%s\n", baseUrl.c_str());
	int posOfEnd = baseUrl.rfind('/');
	if (posOfEnd < 6)
	{
		WriteLog("Url ̫��,����û��/��ֻ��Э�飨http://��:" + pItem->getUrl() + "\n");
		AfxMessageBox(L"Url ̫��,����û��/��ֻ��Э�飨http://��");
		exit(1);
	}
	else
	{
		baseUrl = baseUrl.substr(0, posOfEnd + 1);
	}
	return baseUrl;
}

void CData::setCookie(Cookie& tempCookie)
{
	AcquireSRWLockExclusive(&m_cookieSRW);
	cookie = tempCookie;
	ReleaseSRWLockExclusive(&m_cookieSRW);
}

int CData::getExtractLinkNum()
{
	int tmp = 0;
	AcquireSRWLockExclusive(&m_extractLinkNumSRW);
	tmp = m_extractedLinkNum;
	ReleaseSRWLockExclusive(&m_extractLinkNumSRW);
	return tmp;
}

void CData::addExtractLinkNum()
{
	AcquireSRWLockExclusive(&m_extractLinkNumSRW);
	m_extractedLinkNum++;
	ReleaseSRWLockExclusive(&m_extractLinkNumSRW);
}

int CData::getTestedArgNum()
{
	int tmp = 0;
	AcquireSRWLockExclusive(&m_testedArgNumSRW);
	tmp = m_testedArgNum;
	ReleaseSRWLockExclusive(&m_testedArgNumSRW);
	return tmp;
}

void CData::addTestedArgNum(int val)
{
	AcquireSRWLockExclusive(&m_testedArgNumSRW);
	m_testedArgNum += val;
	ReleaseSRWLockExclusive(&m_testedArgNumSRW);
}

int CData::getTotalArgNum()
{
	return m_totalArgNum;
}

void CData::setTotalArgNum(int val)
{
	m_totalArgNum = val;
}

unsigned CData::crawlerLayer = 100000;





