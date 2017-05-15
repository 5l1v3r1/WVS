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

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	//if (itype == CURLINFO_TEXT)
	//{
	//	//printf("[TEXT]%s\n", pData);  
	//}
	//else if (itype == CURLINFO_HEADER_IN)
	//{
	//	_cprintf("[HEADER_IN]%s\n", pData);
	//}
	//else if (itype == CURLINFO_HEADER_OUT)
	//{
	//	_cprintf("[HEADER_OUT]%s\n", pData);
	//}
	//else if (itype == CURLINFO_DATA_IN)
	//{
	//	//printf("[DATA_IN]%s\n", pData);
	//}
	//else if (itype == CURLINFO_DATA_OUT)
	//{
	//	//printf("[DATA_OUT]%s\n", pData);
	//}
	//_cprintf("\n\n\n*******************************************************************\n\
	//		 		%s\n---------------------------------------------------------------------\n\n\n", pData);
	return 0;
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
	curl_easy_setopt(this->m_pCurl, CURLOPT_VERBOSE, 1);		//���Խ׶Σ�����cmd��������������Ϣ��
	curl_easy_setopt(this->m_pCurl, CURLOPT_DEBUGFUNCTION, &OnDebug);
	curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, &WriteFunction);
	//curl_easy_setopt(this->m_pCurl, CURLOPT_FOLLOWLOCATION, 0);	//�Զ���ת������302���͵Ļظ���
	curl_easy_setopt(this->m_pCurl, CURLOPT_HEADEROPT, 1);
	curl_easy_setopt(this->m_pCurl, CURLOPT_HEADERFUNCTION, &header_callback);
	curl_easy_setopt(this->m_pCurl, CURLOPT_HEADERDATA, &headerStr);
	
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS, m_defaultTimeOut);

	setProxy(s_useProxy, s_proxy);
}


void CHttpClient::setTimeOut(long millsec = -1)
{
	if (millsec == -1)
	{
		curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS,  m_defaultTimeOut);
	}
	else{
		curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS, millsec);
	}
	
}

CURLcode CHttpClient::send(HttpMethod method, const std::string &strCookie, const std::string & strUrl,
						   const std::string & strParam, std::string & strResponse)
{

	m_curCode = CURLE_FTP_WEIRD_SERVER_REPLY; //8
	curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, &strResponse);
	if (strCookie != "")
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_COOKIE, strCookie.c_str());
	}
	if (method == HttpMethod::post)
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(this->m_pCurl, CURLOPT_POST, 1);
		curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, strParam.c_str());
		curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDSIZE, strParam.size());
	}
	else if (HttpMethod::get == method)
	{
		string url;
		if (strParam != "")
		{
			url = strUrl + "?" + strParam;
		}
		else{
			url = strUrl;
		}
		
		curl_easy_setopt(this->m_pCurl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPGET, 1);
	}
	else
	{
		WriteLog("HttpMethod:other");
	}
	return performRequest(strUrl);
}


CURLcode CHttpClient::send(HttpMethod method, const std::string &strCookie, const std::string & strUrl, std::vector<Field> & fieldVec, std::string & strResponse)
{
	m_curCode = CURLE_FTP_WEIRD_SERVER_REPLY; //8
	string args;
	for (unsigned int i = 0; i < fieldVec.size(); i++)
	{
		args += fieldVec[i].toString();
		if (i != fieldVec.size() - 1)
		{
			args += "&";
		}
	}
	if (method == HttpMethod::post)
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(this->m_pCurl, CURLOPT_POST, 1);
		curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, args.c_str());
		curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDSIZE, args.size());
	}
	else if (HttpMethod::get == method)
	{
		string url;
		if (args != "")
		{
			url = strUrl + "?" + args;
		}
		else{
			url = strUrl;
		}
		 
		escapeURL(url);
		curl_easy_setopt(this->m_pCurl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPGET, 1);
	}
	else
	{
		WriteLog("HttpMethod:other");
	}

	curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, &strResponse);
	if (strCookie != "")
	{
		curl_easy_setopt(this->m_pCurl, CURLOPT_COOKIE, strCookie.c_str());
	}
	return performRequest(strUrl);
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


CURLcode CHttpClient::send(HttpMethod method, const std::string &strCookie, const std::string & strUrl, const std::string & strParam,  bool onlyHeader/*=true*/)
{
	curl_easy_setopt(this->m_pCurl, CURLOPT_NOBODY, 1);
	m_curCode = CURLE_FTP_WEIRD_SERVER_REPLY; //8
	curl_easy_setopt(this->m_pCurl, CURLOPT_URL, strUrl.c_str());
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
		//curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPGET, 1);
	}
	else
	{
		WriteLog("HttpMethod:other");
	}
	return performRequest(strUrl);
}

CURLcode CHttpClient::performRequest(string strUrl)
{
	m_curCode = curl_easy_perform(this->m_pCurl);
	if (m_curCode == CURLE_OK){
		curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_statusCode);
		WriteFile("��ַ��_3.txt", to_string(m_statusCode) + "\t" + strUrl);
	}
	else{
		WriteFile("��ַ��_3.txt", "-1\t" + strUrl);
	}
	return m_curCode;
}

void CHttpClient::setProxy(bool useProxy, string proxy)
{
	if (useProxy)
	{
		
		curl_easy_setopt(this->m_pCurl, CURLOPT_PROXY, proxy.c_str());
	}
	else{
		curl_easy_setopt(this->m_pCurl, CURLOPT_PROXY, "");
	}
	//curl_easy_setopt(this->m_pCurl, CURLOPT_PROXY, "127.0.0.1:8888");
}


std::string CHttpClient::s_proxy = "";

bool CHttpClient::s_useProxy = false;


bool CHttpClient::g_INIT_FLAG = false;

int CHttpClient::g_INSTANCE_NUM = 0;

//#include"CHttpClient.h"
//CHttpClient::CHttpClient(){
//	/* CURL *curl_easy_init()
//	@ ��ʼ��curl����CURL *curlָ��
//	*/
//	 this->pCurl= curl_easy_init();
//	if (pCurl == NULL) {
//		std::cout << "curl_easy_init() Err" << std::endl;
//		exit(1);
//	}
//}
//CHttpClient::~CHttpClient(){
//	/* void curl_easy_cleanup(CURL * handle);
//	@ �ͷ�CURL *curlָ��
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
//	 �����󷵻�����input������avg��(avgΪһ��ʼcurl_easy_setopt���õĲ�)  
//	size_t uLen = uSize*uCount;
//	 string &sBuffer = *reinterpret_cast<string *>(avg);  
//	 sBuffer.append(reinterpret_cast<const char *>(input), uLen);  
//	string *pStr = (string *)(avg);
//	pStr->append((char *)(input), uLen);
//
//	 ֻ�з���uSize*uCount�Ż᷵�سɹ�  
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
//	@ ��ȡ������������Ϣ
//	@ info��CURLINFO_RESPONSE_CODE    // ��ȡ���ص�Http��
//	CURLINFO_TOTAL_TIME      // ��ȡ�ܵ���������ʱ��
//	CURLINFO_SIZE_DOWNLOAD   // ��ȡ���ص��ļ���С
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
//	//CURL *pCurl = NULL; //���
//	//CURLcode code;
//
//	///* CURLcode curl_global_init(long flags)
//	//@ ��ʼ��libcurl��ȫ��ֻ���һ��
//	//@ flags: CURL_GLOBAL_DEFAULT     // ��ͬ��CURL_GLOBAL_ALL
//	//CURL_GLOBAL_ALL         // ��ʼ�����еĿ��ܵĵ���
//	//CURL_GLOBAL_SSL         // ��ʼ��֧�ְ�ȫ�׽��ֲ�
//	//CURL_GLOBAL_WIN32       // ��ʼ��win32�׽��ֿ�
//	//CURL_GLOBAL_NOTHING     // û�ж���ĳ�ʼ��
//	//......
//	//*/
//	//code = curl_global_init(CURL_GLOBAL_DEFAULT);
//	//if (code != CURLE_OK) {
//	//	cout << "curl_global_init() Err" << endl;
//	//	return -1;
//	//}
//
//	///* CURL *curl_easy_init()
//	//@ ��ʼ��curl����CURL *curlָ��
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
//	//@ ���Http��Ϣͷ
//	//@ ����string����ʽΪname+": "+contents
//	//*/
//
//	////string header = "username: andyawang";
//	////pHeaders = curl_slist_append(pHeaders, header.c_str());
//
//	///* CURLcode curl_easy_setopt(CURL *handle, CURLoption option, parameter);
//	//@ �����������Լ����ò���
//	//*/
//	//curl_easy_setopt(pCurl, CURLOPT_URL, sUrl.c_str());  // ���ʵ�URL  
//	//curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeaders);  // ����ͷ��(Ҫ��pHeader��û����)  
//	//curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 20);           // ��ʱ(��λS)  
//	//curl_easy_setopt(pCurl, CURLOPT_HEADER, 1);             // �������ݰ���HTTPͷ��  
//
//	//curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, NULL); // !���ݻص�����  
//	//FILE *fp;
//	//   fopen_s(&fp, "d:\\123.html", "w");
//
//
//	//curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, fp);           // !���ݻص������ĲΣ�һ��ΪBuffer���ļ�fd  
//
//	///* CURLcode curl_easy_perform(CURL *handle);
//	//@ ��ʼ����
//	//*/
//	//code = curl_easy_perform(pCurl);
//	//if (code != CURLE_OK) {
//	//	cout << "curl_easy_perform() Err" << endl;
//	//	return -1;
//	//}
//
//	///* CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... );
//	//@ ��ȡ������������Ϣ
//	//@ info��CURLINFO_RESPONSE_CODE    // ��ȡ���ص�Http��
//	//CURLINFO_TOTAL_TIME      // ��ȡ�ܵ���������ʱ��
//	//CURLINFO_SIZE_DOWNLOAD   // ��ȡ���ص��ļ���С
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
//	//@ �ͷ�CURL *curlָ��
//	//*/
//	//curl_easy_cleanup(pCurl);
//
//	///* void curl_global_cleanup(void);
//	//@ ��ʼ��libcurl��ȫ��Ҳֻ���һ��
//	//*/
//	curl_global_cleanup();
//}

