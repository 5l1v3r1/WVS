#pragma once
#include"curl\curl.h"
#include<iostream>
#include<string>
#include "Tool.h"
#include "Field.h"
#include <vector>
using namespace std;

size_t WriteFunction(void *input, size_t uSize, size_t uCount, void *avg);


size_t header_callback(const char  *ptr, size_t size, size_t nmemb, std::string *stream);


class CHttpClient
{
public:
	CHttpClient();
	~CHttpClient();
	void setHeaderOpt(const std::string &strHeaderParam, string& headerStr);
	void setTimeOut(long millsec);

	CURLcode send(HttpMethod method,
		const std::string &strCookie,
		const std::string & strUrl,
		const std::string & strParam,
		std::string & strResponse,
		std::string&strHeader);
	CURLcode send(HttpMethod method,
		const std::string &strCookie,
		const std::string & strUrl,
		vector<Field>& fieldVec,
		std::string & strResponse,
		std::string & strHeader);
	CURLcode send(HttpMethod method,
		const std::string &strCookie,
		const std::string & strUrl,
		vector<Field>& fieldVec,
		std::string & strResponse);
	CURLcode send(HttpMethod method,
		const std::string &strCookie,
		const std::string & strUrl,
		const std::string & strParam,
		std::string & strResponse);
	CURLcode send(HttpMethod method,
		const std::string &strCookie,
		const std::string & strUrl,
		const std::string & strParam,
		bool onlyHeader);

	int getStatusCode() const { return m_statusCode; }
	void setStatusCode(int val) { m_statusCode = val; }
private:
	CURLcode performRequest(string);

	static bool g_INIT_FLAG;
	static int g_INSTANCE_NUM;
	CURL *m_pCurl = NULL;
	CURLcode m_curCode;		
	int m_statusCode = 0;	//当一次调用成功后，返回的状态码。
};