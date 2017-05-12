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
	string cookie;
	string args;
	string argStrs;	//发送的请求参数
	string ext;	
//	vector<string> vecResponse;
};

