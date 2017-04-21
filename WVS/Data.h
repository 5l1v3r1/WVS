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
//全局类，用来存储所有的全局数据。
public:
	CData(string oriUrl);
	~CData();
	bool checkInLinks(Item &des, vector<Item*>&crawlerLinksVec);
	string vecFieldToString(vector<Field> fieldVec);
	void showCrawler();


	void analyseHeader(string& strHeader);
	vector<Item*>* analyseHtml(Item*pItem, string& strHtml);
	Item* getItem();	
	void putItem(Item* pItem);
	vector<Item*>* readLinks();
	void getCookie(Cookie& tempCookie);
	int getRestLinksNum();

	vector<Item*> crawlerLinksItemVec;	//存储所有爬行的links
	Cookie cookie;					//作为最新的cookie，所有线程共一个，每次访问后，都解析http头，一旦发现Cookie改变，则立即使用新的cookie。
	bool cookieDirty = false;
	string htmlFile = "";				//存储所有的网页字符串。测试用。
	string domain = "192.168.8.191/DVWA-master";  //"192.168.8.191/DVWA-master";	//域名常量字符串，作为格式化之后的去重使用。只测试本网站内的网址。
	string originUrl;

	
private:
	SRWLOCK m_linksVecSRW;
	SRWLOCK m_cookieSRW;
	unsigned int crawledNum = 1;					//已经爬行过的链接数量，作为crawlerLinksItemVec的指示器。
};

