#include "stdafx.h"
#include "Data.h"

CData::CData(string oriUrl)
{
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
	
	InitializeSRWLock(&m_linksVecSRW);
	InitializeSRWLock(&m_cookieSRW);

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
		cout << "�����ڱ���վ����ַ��" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().size() >= 5 && (des.getUrl().c_str()[4] == 's' || des.getUrl().c_str()[4] == 'S'))
	{
		cout << "����ַΪhttps,�ݲ�֧��!" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().find("logout") != -1)
	{
		cout << "�˳���½��ַ����Ҫ" << des.getUrl() << endl;
		return true;
	}
	AcquireSRWLockShared(&m_linksVecSRW);
	for (unsigned int i = 0; i < crawlerLinksItemVec.size(); i++)
	{
		if (des.equal(*(crawlerLinksItemVec[i])))
		{
			cout << "��������Ѿ����ڹ���" << des.getUrl() << endl;
			ReleaseSRWLockShared(&m_linksVecSRW);
			return TRUE;
		}
	}
	ReleaseSRWLockShared(&m_linksVecSRW);
	cout << "�����ӣ�" << des.getUrl() << endl;
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
	AcquireSRWLockShared(&m_linksVecSRW);
	cout << "-------------------------------------------------------------------" << endl;
	cout << "url:" << crawlerLinksItemVec[crawledNum]->getUrl().c_str() << endl;
	cout << "cookie:" << cookie.toString() << endl;
	cout << "method:" << crawlerLinksItemVec[crawledNum]->getMethod() << endl;
	cout << "args:" << vecFieldToString(crawlerLinksItemVec[crawledNum]->getArgs()) << endl;
	cout << crawledNum << endl;
	cout << "-------------------------------------------------------------------" << endl << endl;
	WriteFile("��ַ��.txt", crawlerLinksItemVec[crawledNum]->getUrl() + string("\tMethod:") + (char)(crawlerLinksItemVec[crawledNum]->getMethod() + '0') + string("\tAEGUMENT:") + vecFieldToString(crawlerLinksItemVec[crawledNum]->getArgs()));
	ReleaseSRWLockShared(&m_linksVecSRW);
}


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
	if (!(cookie == newCookie))
	{
		cookie = newCookie;
	//	_cprintf("cookie:%s\n", newCookie.toString().c_str());
	}
	ReleaseSRWLockExclusive(&m_cookieSRW);
	strHeader = "";
}

vector<Item*>* CData::analyseHtml(Item*pItem, string& strHtml)
{
	vector<Item*> *pItemVec = new vector<Item*>();
	vector<string>linksVec;		//�ݴ� ��һ����ҳ����ȡ�Ķ��links��
	vector<string>baseVec;		//�ݴ� ��һ����ҳ����ȡ��base
	vector<string>formStrVec;	//�ݴ�  form
	vector<HtmlForm>formVec;	//�ݴ� 
	//1. ��ȡbaseUrl�� ����Ѱ��base��ǩ�����û�����ȡ����ҳ��ַȥ�����һ���á�\���ָ������ַ�����Ϊbaseurl��
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
	_cprintf("baseUrl:%s\n", baseUrl.c_str());
	int posOfEnd = baseUrl.rfind('/');
	if (posOfEnd < 6)
	{
		WriteLog("Url ̫��,����û��/��ֻ��Э�飨http://��:" + pItem->getUrl() + "\n");
		exit(1);
	}
	else	
	{
		baseUrl = baseUrl.substr(0, posOfEnd + 1);
	}

	//2. ��ȡ��ҳ�����е�<a>��ǩ����ȡ���е�href�ֶΣ���ʽ��ΪItem,��������ʽΪget��ȥ�رȽϣ������ظ�������ɨ����С�
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
		if (tempLink.find("login.php") != -1){
			int x = 1;
			x++;
		}
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
			pItemVec->push_back(pTempNewItem);
			putItem(pTempNewItem);
		}
		else
		{
			//	cout << "��������Ѿ����ڹ���" << pTempNewItem->getUrl() << endl;
			delete pTempNewItem;
		}
	}

	//3. ��ȡ���е�form������ÿ�����е�input��ǩ������ȡ����ʽ��ΪField�����ÿ�����洢ΪItem,��������ʽΪpost��ȥ�رȽϣ������ظ�������ɨ����С�
	findByRegex(strHtml, HtmlForm::FORM_REGEX, formStrVec, false);
	HtmlForm* form;
	string debugtemp;
	for (unsigned int i = 0; i < formStrVec.size(); i++)
	{
		 debugtemp = formStrVec[i];
		form = new HtmlForm(formStrVec[i]);
		//cout << form->toString().c_str() << endl;
		formVec.push_back(*form);
	}
	for (unsigned int i = 0; i < formVec.size(); i++)
	{
		string temp = formVec[i].getAction();
		tempLink = temp;
		pTempNewItem = new Item();
		formatLink(baseUrl, tempLink, argStr);
		if (tempLink.find("login.php") != -1){
			int x = 1;
			x++;
		}
		pTempNewItem->setUrl(tempLink);
		pTempNewItem->setMethod(HttpMethod::post);
		pTempNewItem->setArgs(formVec[i].getFields());
		if (!checkInLinks(*pTempNewItem, crawlerLinksItemVec))
		{
			pTempNewItem->setLayer(pItem->getLayer() + 1);
		//	crawlerLinksItemVec.push_back(*pTempNewItem);	//mutex area
		//	CExtractJob *job = new CExtractJob(pTempNewItem, this);
		//	pThreadManage->Run(job, NULL);
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
	baseVec.clear();
	formStrVec.clear();
	return pItemVec;
}

Item* CData::getItem()
{
	Item * pItme;
	AcquireSRWLockExclusive(&m_linksVecSRW);
	if (crawlerLinksItemVec.size() <= crawledNum)
	{
		pItme = NULL;
	}
	else{
		pItme = crawlerLinksItemVec[crawledNum];
		crawledNum++;
	}
	ReleaseSRWLockExclusive(&m_linksVecSRW);
	return pItme;
}

void CData::putItem(Item* pItem)
{
	AcquireSRWLockExclusive(&m_linksVecSRW);
	crawlerLinksItemVec.push_back(pItem);
	ReleaseSRWLockExclusive(&m_linksVecSRW);
}

void CData::getCookie(Cookie& tempCookie)
{
	AcquireSRWLockShared(&m_cookieSRW);
	tempCookie = cookie;
	if (tempCookie.toString() == ""){
		int x = 1;
		x++;
	}
	ReleaseSRWLockShared(&m_cookieSRW);
}

int CData::getRestLinksNum()
{
	int num;
	AcquireSRWLockExclusive(&m_linksVecSRW);
	num = crawlerLinksItemVec.size();
	ReleaseSRWLockExclusive(&m_linksVecSRW);
	return num;
}

