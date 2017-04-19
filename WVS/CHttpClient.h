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

private:
	static bool g_INIT_FLAG;
	static int g_INSTANCE_NUM;
	CURL *m_pCurl = NULL;
	CURLcode code;
};