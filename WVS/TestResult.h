#pragma once
#include <string>
#include <vector>
using namespace std;
class TestResult
{
public:
	TestResult();

	int id;
	int itemId;
	int type;
	int caseId;
	int injectPos;
	int resultState;
	int method;
	string url;
	string cookie;	//测试时使用的cookie
	string args;	//原始参数
	string argStrs;	//发送的请求参数
	string ext;		//无用字段
//	vector<string> vecResponse;
};

