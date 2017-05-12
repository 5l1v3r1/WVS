#pragma once
#include <string>
#include <vector>
#include "Tool.h"
#include "Cookie.h"
#include "Item.h"
#include "HtmlForm.h"
#include "MyThreadPool.h"

class CData
{
//ȫ���࣬�����洢���е�ȫ�����ݡ�
public:
	CData(string oriUrl);
	CData();
	~CData();
	bool checkInLinks(Item &des, vector<Item*>&crawlerLinksVec);
	string vecFieldToString(vector<Field> fieldVec);
	void showCrawler();


	void analyseHeader(string& strHeader);
	vector<Item*>* analyseHtml(Item*pItem, string& strHtml);
	Item* analyseRedirectHeader(Item* pItem, string headerStr);
	//Item* getItem();	
	Item* getItemByIndex(unsigned index);
	void putItem(Item* pItem);
	vector<Item*>* readLinks();
	void getCookie(Cookie& tempCookie);
	void setCookie(Cookie& tempCookie);
	string getBaseUrl(string strHtml, Item *pItem);
	void setUrl(string oriUrl);

	int getExtractLinkNum();
	void addExtractLinkNum();
	int getTestedArgNum();
	void addTestedArgNum(int val);
	int getTotalArgNum();
	void setTotalArgNum(int val);

	vector<Item*> crawlerLinksItemVec;		//�洢�������е�links
	Cookie cookie;							//��Ϊ���µ�cookie�������̹߳�һ����ÿ�η��ʺ󣬶�����httpͷ��һ������Cookie�ı䣬������ʹ���µ�cookie��
	bool cookieDirty = false;
	string htmlFile = "";					//�洢���е���ҳ�ַ����������á�
	string domain = "";						//"192.168.8.191/DVWA-master";	//���������ַ�������Ϊ��ʽ��֮���ȥ��ʹ�á�ֻ���Ա���վ�ڵ���ַ��
	string originUrl;
	static unsigned crawlerLayer;

private:
	SRWLOCK m_linksVecSRW;
	SRWLOCK m_cookieSRW;
	SRWLOCK m_extractLinkNumSRW;
	SRWLOCK m_testedArgNumSRW;
	unsigned int crawledNum = 1;					//�Ѿ����й���������������ΪcrawlerLinksItemVec��ָʾ����
	bool hasSetUrl = false;
	int m_extractedLinkNum = 0;
	int m_totalArgNum = 0;
	int m_testedArgNum = 0;

	void initData();
	
};

