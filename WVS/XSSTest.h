#pragma once
#include "Test.h"
#include "Tool.h"
#include "Data.h"
typedef struct XSSResult{
	int id;
	int caseId;
	int itemId;
	int injectPos;
	int resultState;
	int type;
	string url;
	string cookie;
	string args;
	string ext;	//用于基于bool的注入记录详情； 格式为。caseId:length;
	~XSSResult(){
		_cprintf("~XSSResult()");
	}
}XSSResult;
class XSSTest: public Test
{
public:
	XSSTest();
	XSSTest(CData* pData);
	~XSSTest();
	virtual bool loadConfiguration(string fileName = "XSSConf.xml");
	virtual bool saveConfiguration(string fileName = "XSSConf.xml");
	virtual bool test(CHttpClient *pHttpClient, Item *pItem);
	virtual void putResultItem(void*ptr);
	virtual string resultToString();
	virtual string resultToStringForCSV();
	void clearResult();

private:
	SRWLOCK m_resultSRW;
	vector<XSSResult*> m_vecpResult;
	CData* m_pData;
};

