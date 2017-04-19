#include "stdafx.h"
#include "CHttpClient.h"

size_t WriteFunction(void *input, size_t uSize, size_t uCount, void *avg)
{
	size_t uLen = uSize*uCount;
	string *pStr = (string *)(avg);
	pStr->append((char *)(input), uLen);
	return uLen;
}

size_t header_callback(const char  *ptr, size_t size, size_t nmemb, std::string *stream)
{
	size_t len = size * nmemb;
	stream->append(ptr, len);
	return len;
}

CHttpClient::CHttpClient()
{
	if (!this->g_INIT_FLAG)
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}
	this->g_INSTANCE_NUM++;
	this->m_pCurl = curl_easy_init();
	curl_easy_setopt(this->m_pCurl, CURLOPT_NOSIGNAL, 1L);

}

CHttpClient::~CHttpClient()
{
	curl_easy_cleanup(m_pCurl);


	this->g_INSTANCE_NUM--;
	if (this->g_INSTANCE_NUM == 0 && this->g_INIT_FLAG)
	{
		this->g_INIT_FLAG = false;
		curl_global_cleanup();
	}
}
  
void CHttpClient::setHeaderOpt(const std::string &strHeaderParam, string& headerStr)
{
	curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, "Accept: text/html, application/xhtml+xml, */*");
	chunk = curl_slist_append(chunk, "Accept-Language: zh-CN");
	//chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/535.7 (KHTML, like Gecko)");
	chunk = curl_slist_append(chunk, "User - Agent: Mozilla / 5.0 (Windows NT 10.0; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 56.0.2924.87 Safari / 537.36");
	chunk = curl_slist_append(chunk, "Connection: keep-alive");
	curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(this->m_pCurl, CURLOPT_VERBOSE, 1);		//调试阶段，会在cmd中输出链接相关信息。
	curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, &WriteFunction);
	curl_easy_setopt(this->m_pCurl, CURLOPT_FOLLOWLOCATION, 1);	//自动跳转，处理302类型的回复。
	curl_easy_setopt(this->m_pCurl, CURLOPT_HEADEROPT, 1);
	curl_easy_setopt(this->m_pCurl, CURLOPT_HEADERFUNCTION, &header_callback);
	curl_easy_setopt(this->m_pCurl, CURLOPT_HEADERDATA, &headerStr);
}


CURLcode CHttpClient::send(HttpMethod method, const std::string &strCookie, const std::string & strUrl, const std::string & strParam, std::string & strResponse)
{
	CURLcode code = CURLE_FTP_WEIRD_SERVER_REPLY; //8
	curl_easy_setopt(this->m_pCurl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, &strResponse);
	if (strCookie != "")
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_COOKIE, strCookie.c_str());
	}
	if (method == HttpMethod::post)
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_POST, 1);
		curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, strParam.c_str());
	}
	else if (HttpMethod::get == method)
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPGET, 1);
	}
	else
	{

	}

	code = curl_easy_perform(this->m_pCurl);
	if (code == CURLE_OK){
		int statu;
		curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &statu);
		WriteFile("网址树_3.txt", to_string(statu) + "\t" + strUrl);
	}
	else{
		WriteFile("网址树_3.txt", "-1\t" + strUrl);
	}
	return code;
}


CURLcode CHttpClient::send(HttpMethod method, const std::string &strCookie, const std::string & strUrl, std::vector<Field> & fieldVec, std::string & strResponse)
{
	CURLcode code = CURLE_FTP_WEIRD_SERVER_REPLY; //8
	string args;
	curl_easy_setopt(this->m_pCurl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, &strResponse);
	if (strCookie != "")
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_COOKIE, strCookie.c_str());
	}
	if (method == HttpMethod::post)
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_POST, 1);

		for (unsigned  int i = 0; i < fieldVec.size(); i++)
		{
			args += fieldVec[i].toString();
			if (i != fieldVec.size() - 1)
			{
				args += "&";
			}
		}
		cout << "args:" << args << endl;
		curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, args.c_str());
		//	curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDSIZE, args.size());
	}
	else if (HttpMethod::get == method)
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPGET, 1);
	}
	else
	{
		WriteLog("HttpMethod:other");
	}

	code = curl_easy_perform(this->m_pCurl);
	if (code == CURLE_OK){
		int statu;
		curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &statu);
		WriteFile("网址树_3.txt", to_string(statu) + "\t" + strUrl);
	}
	else{
		WriteFile("网址树_3.txt", "-1\t" + strUrl);
	}
	return code;
	//return curl_easy_perform(this->m_pCurl);
}

CURLcode CHttpClient::send(HttpMethod method, const std::string &strCookie, const std::string & strUrl, std::vector<Field> & fieldVec, std::string & strResponse, std::string &strHeader)
{
	curl_easy_setopt(this->m_pCurl, CURLOPT_HEADERDATA, &strHeader);
	return send(method, strCookie, strUrl, fieldVec, strResponse);
}

CURLcode CHttpClient::send(HttpMethod method, const std::string &strCookie, const std::string & strUrl, const std::string&strParam, std::string & strResponse, std::string &strHeader)
{
	curl_easy_setopt(this->m_pCurl, CURLOPT_HEADERDATA, &strHeader);
	return send(method, strCookie, strUrl, strParam, strResponse);
}


bool CHttpClient::g_INIT_FLAG = false;

int CHttpClient::g_INSTANCE_NUM = 0;

//#include"CHttpClient.h"
//CHttpClient::CHttpClient(){
//	/* CURL *curl_easy_init()
//	@ 初始化curl生成CURL *curl指针
//	*/
//	 this->pCurl= curl_easy_init();
//	if (pCurl == NULL) {
//		std::cout << "curl_easy_init() Err" << std::endl;
//		exit(1);
//	}
//}
//CHttpClient::~CHttpClient(){
//	/* void curl_easy_cleanup(CURL * handle);
//	@ 释放CURL *curl指针
//	*/
//	curl_easy_cleanup(pCurl);
//}
//
//static size_t WriteFunction(void *input, size_t uSize, size_t uCount, void *avg)
//{
//	cout << "[WriteFunction]:" << endl
//		<< "input=" << (char*)input << endl
//		<< "elementSize=" << uSize << endl
//		<< "elementCount=" << uCount << endl
//		<< "input Addr:" << input << endl;
//
//	 将请求返回数据input拷贝到avg中(avg为一开始curl_easy_setopt设置的参)  
//	size_t uLen = uSize*uCount;
//	 string &sBuffer = *reinterpret_cast<string *>(avg);  
//	 sBuffer.append(reinterpret_cast<const char *>(input), uLen);  
//	string *pStr = (string *)(avg);
//	pStr->append((char *)(input), uLen);
//
//	 只有返回uSize*uCount才会返回成功  
//	return uLen;
//}
//
//************************************
// Method:    send
// FullName:  CHttpClient::send
// Access:    public 
// Returns:   CURLcode
// Qualifier:
// Parameter: int protocol 
// Parameter: int method
// Parameter: const std::string & strUrl
// Parameter: const std::s	std::string &strResponse){
//	if (method == 1){
//		curl_easy_setopt(this->pCurl, CURLOPT_URL, strUrl.c_str());
//		if (strHeaderParam == ""  && strHeaderParam.length()!=0)
//			curl_easy_setopt(this->pCurl, CURLOPT_HEADER, strHeaderParam.c_str());
//		if (strCookie != NULL && strCookie.length() != 0)
//			curl_easy_setopt(this->pCurl, CURLOPT_COOKIE, strCookie.c_str());
//
//		curl_easy_setopt(this->pCurl, CURLOPT_POSTFIELDS, strParam.c_str());
//		curl_easy_setotring & strParam
// Parameter: std::string & strResponse
//************************************
//
//CURLcode CHttpClient::send(int protocol,
//							int method, 
//							const std::string &strUrl, 
//							const std::string &strHeaderParam, 
//							const std::string &strCookie,
//							const std::string &strParam, 
//						pt(this->pCurl, CURLOPT_WRITEFUNCTION, &WriteFunction);
//	}
//	else{
//
//	}
//	this->code = curl_easy_perform(this->pCurl);
//	if (code != CURLE_OK) {
//		std::cout << "curl_easy_perform() Err" << std::endl;
//		return CURLE_HTTP_POST_ERROR;
//	}
//
//	/* CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... );
//	@ 获取下载完的相关信息
//	@ info：CURLINFO_RESPONSE_CODE    // 获取返回的Http码
//	CURLINFO_TOTAL_TIME      // 获取总的请求下载时间
//	CURLINFO_SIZE_DOWNLOAD   // 获取下载的文件大小
//	......
//	*/
//	long retCode = 0;
//	code = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &retCode);
//	if (code != CURLE_OK) {
//		cout << "curl_easy_perform() Err" << endl;
//		return -1;
//	}
//	cout << "[Http Return Code] : " << retCode << endl;
//	cout << "[Http Context] : " << endl << sBuffer << endl;
//
//	return CURLE_OK;
//}






//int main(){
//
//	cout << curl_global_init(CURL_GLOBAL_DEFAULT) << endl;
//	CHttpClient* mhc = new CHttpClient();
//	string sBuffer;
//	mhc->send(0, 1, "115.28.63.139/HNHDC", NULL, NULL, NULL, sBuffer);
//	cout << sBuffer << endl;
//	mhc->~CHttpClient();
//
//	//CURL *pCurl = NULL; //句柄
//	//CURLcode code;
//
//	///* CURLcode curl_global_init(long flags)
//	//@ 初始化libcurl，全局只需调一次
//	//@ flags: CURL_GLOBAL_DEFAULT     // 等同于CURL_GLOBAL_ALL
//	//CURL_GLOBAL_ALL         // 初始化所有的可能的调用
//	//CURL_GLOBAL_SSL         // 初始化支持安全套接字层
//	//CURL_GLOBAL_WIN32       // 初始化win32套接字库
//	//CURL_GLOBAL_NOTHING     // 没有额外的初始化
//	//......
//	//*/
//	//code = curl_global_init(CURL_GLOBAL_DEFAULT);
//	//if (code != CURLE_OK) {
//	//	cout << "curl_global_init() Err" << endl;
//	//	return -1;
//	//}
//
//	///* CURL *curl_easy_init()
//	//@ 初始化curl生成CURL *curl指针
//	//*/
//	//pCurl = curl_easy_init();
//	//if (pCurl == NULL) {
//	//	cout << "curl_easy_init() Err" << endl;
//	//	return -1;
//	//}
//
//	//string sUrl = "115.28.63.139/TestHTML/TestH/t11_webWorker.html";
//	//curl_slist *pHeaders = NULL;
//	//string sBuffer;
//
//	///*struct curl_slist *curl_slist_append(struct curl_slist * list, const char * string);
//	//@ 添加Http消息头
//	//@ 属性string：形式为name+": "+contents
//	//*/
//
//	////string header = "username: andyawang";
//	////pHeaders = curl_slist_append(pHeaders, header.c_str());
//
//	///* CURLcode curl_easy_setopt(CURL *handle, CURLoption option, parameter);
//	//@ 设置下载属性及常用参数
//	//*/
//	//curl_easy_setopt(pCurl, CURLOPT_URL, sUrl.c_str());  // 访问的URL  
//	//curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeaders);  // 属性头部(要不pHeader就没用了)  
//	//curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 20);           // 超时(单位S)  
//	//curl_easy_setopt(pCurl, CURLOPT_HEADER, 1);             // 下载数据包括HTTP头部  
//
//	//curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, NULL); // !数据回调函数  
//	//FILE *fp;
//	//   fopen_s(&fp, "d:\\123.html", "w");
//
//
//	//curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, fp);           // !数据回调函数的参，一般为Buffer或文件fd  
//
//	///* CURLcode curl_easy_perform(CURL *handle);
//	//@ 开始下载
//	//*/
//	//code = curl_easy_perform(pCurl);
//	//if (code != CURLE_OK) {
//	//	cout << "curl_easy_perform() Err" << endl;
//	//	return -1;
//	//}
//
//	///* CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... );
//	//@ 获取下载完的相关信息
//	//@ info：CURLINFO_RESPONSE_CODE    // 获取返回的Http码
//	//CURLINFO_TOTAL_TIME      // 获取总的请求下载时间
//	//CURLINFO_SIZE_DOWNLOAD   // 获取下载的文件大小
//	//......
//	//*/
//	//long retCode = 0;
//	//code = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &retCode);
//	//if (code != CURLE_OK) {
//	//	cout << "curl_easy_perform() Err" << endl;
//	//	return -1;
//	//}
//	//cout << "[Http Return Code] : " << retCode << endl;
//	//cout << "[Http Context] : " << endl << sBuffer << endl;
//
//	//cout << "sbuffer   addr = "<<&sBuffer <<"\tsBuffer[1]"<<sBuffer[0]<<endl;
//	///* void curl_easy_cleanup(CURL * handle);
//	//@ 释放CURL *curl指针
//	//*/
//	//curl_easy_cleanup(pCurl);
//
//	///* void curl_global_cleanup(void);
//	//@ 初始化libcurl，全局也只需调一次
//	//*/
//	curl_global_cleanup();
//}

