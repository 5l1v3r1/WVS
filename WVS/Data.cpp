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
// Returns:   bool   ���ڹ���true;    δ���ֹ�false��
// Parameter: Item & des :
// Parameter: vector<Item> & crawlerLinksVec :
// Function:  �鿴�Ƿ��Ѿ����ڹ���
//************************************
bool CData::checkInLinks(Item &des, vector<Item*>&crawlerLinksVec)
{
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
	m_itemMutex.Lock();
	for (unsigned int i = 0; i < crawlerLinksItemVec.size(); i++)
	{
		if (des.equal(*(crawlerLinksItemVec[i])))
		{
			cout << "��������Ѿ����ڹ���" << des.getUrl() << endl;
			return true;
		}
	}
	m_itemMutex.Unlock();
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
	m_itemMutex.Lock();
	cout << "-------------------------------------------------------------------" << endl;
	cout << "url:" << crawlerLinksItemVec[crawledNum]->getUrl().c_str() << endl;
	cout << "cookie:" << cookie.toString() << endl;
	cout << "method:" << crawlerLinksItemVec[crawledNum]->getMethod() << endl;
	cout << "args:" << vecFieldToString(crawlerLinksItemVec[crawledNum]->getArgs()) << endl;
	cout << crawledNum << endl;
	cout << "-------------------------------------------------------------------" << endl << endl;
	WriteFile("��ַ��.txt", crawlerLinksItemVec[crawledNum]->getUrl() + string("\tMethod:") + (char)(crawlerLinksItemVec[crawledNum]->getMethod() + '0') + string("\tAEGUMENT:") + vecFieldToString(crawlerLinksItemVec[crawledNum]->getArgs()));
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
	vector<string>linksVec;	//�ݴ� ��һ����ҳ����ȡ�Ķ��links��
	vector<string>baseVec;	//�ݴ� ��һ����ҳ����ȡ��base
	vector<string>formStrVec;//�ݴ�  form
	vector<HtmlForm>formVec; //�ݴ� 
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
			//	cout << "��������Ѿ����ڹ���" << pTempNewItem->getUrl() << endl;
			delete pTempNewItem;
		}
	}

	//3. ��ȡ���е�form������ÿ�����е�input��ǩ������ȡ����ʽ��ΪField�����ÿ�����洢ΪItem,��������ʽΪpost��ȥ�رȽϣ������ظ�������ɨ����С�
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


