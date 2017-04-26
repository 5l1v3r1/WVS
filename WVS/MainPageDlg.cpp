// MainPageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WVS.h"
#include "MainPageDlg.h"
#include "afxdialogex.h"

// CMainPageDlg �Ի���

IMPLEMENT_DYNAMIC(CMainPageDlg, CDialogEx)

void cls(void)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 }; //���λ��  
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// ��ǰbuf�е�����cell��Ŀ  
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// ��������Ļ���ո�  
	if (!FillConsoleOutputCharacter(hConsole,
		TEXT(' '),
		dwConSize,
		coordScreen,
		&cCharsWritten))
	{
		return;
	}
	//��ȡ��������  
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// ����buf����  
	if (!FillConsoleOutputAttribute(hConsole,
		csbi.wAttributes,
		dwConSize,
		coordScreen,
		&cCharsWritten))
	{
		return;
	}
	// ������ƶ���ʼλ��  
	SetConsoleCursorPosition(hConsole, coordScreen);
}

// SetConsoleMode();  
BOOL SetConsoleMode(int cols, int lines)
{
	BOOL retval = FALSE;
	HANDLE handle = ::CreateFile(
		TEXT("CONOUT$"),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return retval;
	}

	// ��ȡԭʼ��С  
	CONSOLE_SCREEN_BUFFER_INFO OldConsoleScreenBufferInfo = { 0 };
	if (!GetConsoleScreenBufferInfo(handle, &OldConsoleScreenBufferInfo))
	{
		CloseHandle(handle);
		return retval;
	}

	// ��ȡԭ��buf����  
	DWORD dwsize = OldConsoleScreenBufferInfo.dwSize.X*OldConsoleScreenBufferInfo.dwCursorPosition.Y + OldConsoleScreenBufferInfo.dwCursorPosition.X;
	LPWSTR buf = new WCHAR[dwsize];
	COORD readpos = { 0, 0 };
	DWORD dwBytes = 0;
	ReadConsoleOutputCharacter(handle, buf, dwsize, readpos, &dwBytes);

	do
	{
		SMALL_RECT rect;
		rect.Left = 0;
		rect.Top = 0;
		rect.Right = cols - 1;
		rect.Bottom = lines - 1;
		SetConsoleWindowInfo(handle, TRUE, &rect);

		// ����buf  
		COORD dwnewPosition;
		dwnewPosition.X = cols;
		dwnewPosition.Y = lines;
		SetConsoleScreenBufferSize(handle, dwnewPosition);

		// �����´��ڴ�С  
		CONSOLE_SCREEN_BUFFER_INFO NewConsoleScreenBufferInfo = { 0 };
		if (!GetConsoleScreenBufferInfo(handle, &NewConsoleScreenBufferInfo))
			break;
		if (lines >= 80)
		{
			rect.Right = NewConsoleScreenBufferInfo.dwMaximumWindowSize.X - 1;
		}
		else
		{
			rect.Right = (lines << 1);
		}

		if (cols >= 25)
		{
			rect.Bottom = NewConsoleScreenBufferInfo.dwMaximumWindowSize.Y - 1;
		}
		else
		{
			rect.Bottom = (cols >> 1);
		}

		if (SetConsoleWindowInfo(handle, TRUE, &rect))
		{
			retval = TRUE;
		}
	} while (0);

	CloseHandle(handle);

	// д��ԭʼ����   
	DWORD curpos = 0;
	HANDLE outhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	cls();
	for (unsigned int i = 0; i < dwsize; i += OldConsoleScreenBufferInfo.dwSize.X)
	{
		BOOL newline = FALSE;
		if (dwsize - i < 79)
		{
			WriteFile(outhandle, buf + i, dwsize - i, &dwBytes, NULL);
			break;
		}
		DWORD dwwritebyte = 79;
		while (*(buf + i + dwwritebyte) == 0x20)
		{
			newline = TRUE;
			dwwritebyte--;
		}
		WriteFile(outhandle, buf + i, dwwritebyte + 1, &dwBytes, NULL);
		if (newline)
		{
			WriteFile(outhandle, "\r\n", 2, &dwBytes, NULL);
		}
	}
	delete[]buf;
	return retval;
}


CMainPageDlg::CMainPageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainPageDlg::IDD, pParent)
	, m_totalNum(0)
	, m_crawledNum(_T(""))
	, m_totalTestNum(0) 
	, m_strOriUrl(_T(""))
	, m_usedTime(0)
{
	string fileName = "��ַ\\"+to_string(time(NULL));
	system(("mkdir " + fileName).c_str());
	system(("move ��ַ��* " + fileName ).c_str());
	WriteLog("------------------------------------------�µ�һ��ɨ��" +to_string(time(NULL))+ "-----------------------------------------------------");
	
}

CMainPageDlg::~CMainPageDlg()
{


}

void CMainPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_urlTree);
	DDX_Text(pDX, IDC_TotalNum_EDIT, m_totalNum);
	DDX_Text(pDX, IDC_CrawledNum_EDIT, m_crawledNum);
	DDX_Text(pDX, IDC_TotalTestNum_EDIT, m_totalTestNum);
	DDX_Text(pDX, IDC_EDIT1, m_strOriUrl);
	DDX_Control(pDX, ID_BEGIN, m_btnBegin);
	DDX_Text(pDX, IDC_UsedTime_EDIT, m_usedTime);
}


BEGIN_MESSAGE_MAP(CMainPageDlg, CDialogEx)
	
	ON_EN_CHANGE(IDC_NETADDRESS1, &CMainPageDlg::OnEnChangeNetaddress1)
	ON_BN_CLICKED(ID_BEGIN, &CMainPageDlg::OnBnClickedBegin)
	ON_MESSAGE(WM_MY_MONITOR, &CMainPageDlg::OnMONITOR)
END_MESSAGE_MAP()

// CMainPageDlg ��Ϣ�������



void CMainPageDlg::OnEnChangeNetaddress1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//m_oriUrl.DisplayErrorTip();
}


LRESULT CMainPageDlg::OnMONITOR(WPARAM wParam, LPARAM lParam)
{
	CString cStr;
	switch (wParam){
		case 0:{
				   m_usedTime = lParam;
				   SetDlgItemText(ID_BEGIN, L"����");
				   m_workState = 0;	//�����ˡ�
				   _cprintf("%s\n used Time:%d\n", pSQLiTest->resultToString().c_str(), (clock() - start) / CLOCKS_PER_SEC);
				  // _cprintf("%s*****", pSQLiTest->resultToString().c_str());
				break;
		}
		case 1:{
				   m_usedTime = lParam;
		}
		default:{
					WriteLog("monitor other command:" + to_string(wParam));
		}
	}
	UpdateData(FALSE);
	return NULL;
}

size_t header_callback2(const char  *ptr, size_t size, size_t nmemb, std::string *stream)
{
	size_t len = size * nmemb;
	stream->append(ptr, len);
	return len;
}
size_t WriteFunction2(void *input, size_t uSize, size_t uCount, void *avg)
{
	size_t uLen = uSize*uCount;
	string *pStr = (string *)(avg);
	pStr->append((char *)(input), uLen);
	return uLen;
}

void CMainPageDlg::OnBnClickedBegin()
{
	SetConsoleMode(200, 100);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	//const int bufInlen = 100;
	//char buf[bufInlen];
	//WideCharToMultiByte(CP_ACP, 0, m_strOriUrl, -1, buf, bufInlen, NULL, NULL);
	//strcpy_s(buf, 100, (string("http://192.168.8.191/sqli-labs-master/")).c_str());
	//_cprintf("%d,%d,%s,%d\n", m_totalNum, m_totalTestNum, buf, sizeof(buf));
	//m_pData = new CData(buf);

	m_workState =-2;	//testState: -2,   normalState =1;
	pSQLiTest = new CSQLiTest(m_pData);
	pSQLiTest->saveConfiguration();
	//vector<Field> arg;
	//Field field("uname", "");
	//arg.push_back(field);
	//Field field2("passwd", "");
	//arg.push_back(field2);
	//Field field3("submit", "Submit");
	//arg.push_back(field3);
	//Item *pItem = new Item(HttpMethod::post, "http://192.168.8.191/sqli-labs-master/Less-15/",arg);
	//CHttpClient *pHttp = new CHttpClient();
	//string headerStr;
	//pHttp->setHeaderOpt("", headerStr);
	//pSQLiTest->test(pHttp, pItem);
	//_cprintf("%s\n used Time:%d\n", pSQLiTest->resultToString().c_str(), (clock()- start)/CLOCKS_PER_SEC);


	if (m_workState > 0)
	{
		if (firstFlag == 1)
		{
			start = clock();
			firstFlag = FALSE;
			//��һ�Σ���ʼ��
			UpdateData(TRUE);
			m_totalNum = 100;
			m_crawledNum = "50(%50)";
			m_totalTestNum = 200;
			const int bufInlen = 100;
			char buf[bufInlen];
			WideCharToMultiByte(CP_ACP, 0, m_strOriUrl, -1, buf, bufInlen, NULL, NULL);
			strcpy_s(buf, 100, (string("http://192.168.8.191/DVWA-master/")).c_str());
			_cprintf("%d,%d,%s,%d\n", m_totalNum, m_totalTestNum, buf, sizeof(buf));
			m_pData = new CData(buf);
			m_pThreadPool = new CMyThreadPool(8);
			pSQLiTest = new CSQLiTest(m_pData);

			Item *pItem = new Item(HttpMethod::get, buf);
			MonitorJob *pMJob = new MonitorJob(this->m_hWnd, start, m_pThreadPool);
			m_pThreadPool->addJob(pMJob, NULL);
			CExtractJob *pJob = new CExtractJob(pItem, m_pData,pSQLiTest);
			m_pThreadPool->addJob(pJob, NULL);
		}
		else{
			//����ִ������
			_cprintf("continue\n");
			m_pThreadPool->resume();
			SetDlgItemText(ID_BEGIN, L"��ͣ");
		}
		m_workState = -1;
	}
	else if(m_workState == -1){
		//��ͣ��
		_cprintf("pause\n");
		m_pThreadPool->pause();
		SetDlgItemText(ID_BEGIN, L"��ʼ");
		m_workState = 2;
	}
	else if(m_workState == 0){
		AfxMessageBox(L"�Ѿ������ˣ�");
	}
	else{
		CString cstr;
		//cstr.Format(L"�쳣״̬!%d", m_workState);
		//AfxMessageBox(cstr);
	}
}





/*

//����ֻ����ͷ����
//pSQLiTest = new CSQLiTest(m_pData);
//pSQLiTest->saveConfiguration();
//pSQLiTest->loadConfiguration();
string headerStr;
//curl_global_init(CURL_GLOBAL_ALL);
//CURL *curl = curl_easy_init();
//if (curl) {
//	curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.8.191/sqli-labs-master/");
//	/* get us the resource without a body! */
//	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

//	curl_slist *chunk = NULL;
//	chunk = curl_slist_append(chunk, "Accept: text/html, application/xhtml+xml, */*");
//	chunk = curl_slist_append(chunk, "Accept-Language: zh-CN");
//	//chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/535.7 (KHTML, like Gecko)");
//	chunk = curl_slist_append(chunk, "User - Agent: Mozilla / 5.0 (Windows NT 10.0; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 56.0.2924.87 Safari / 537.36");
//	chunk = curl_slist_append(chunk, "Connection: keep-alive");
//	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
//	curl_easy_setopt(curl, CURLOPT_HEADEROPT, 1);
//	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &header_callback2);
//	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerStr);
//	curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
//	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteFunction2);


//	
//	/* Perform the request */
//	curl_easy_perform(curl);
//}
/*
// CHttpClient *pHttp = new CHttpClient();
// pHttp->setHeaderOpt("", headerStr);

pHttp->send(HttpMethod::get, "", "http://192.168.8.191/sqli-labs-master/Less-11/?uname='&passwd=&submit=Submit", "", TRUE);


*/