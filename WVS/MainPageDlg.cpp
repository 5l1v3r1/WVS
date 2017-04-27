// MainPageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WVS.h"
#include "MainPageDlg.h"
#include "afxdialogex.h"

// CMainPageDlg �Ի���

IMPLEMENT_DYNAMIC(CMainPageDlg, CDialogEx)


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
				   WriteFile("��ַ���������Խ��.csv", pSQLiTest->resultToStringForCSV());
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


void CMainPageDlg::OnBnClickedBegin()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	//m_workState =-2;	//testState: -2,   normalState =1;
	

	/*start = clock();
	const int bufInlen = 100;
	char buf[bufInlen];
	WideCharToMultiByte(CP_ACP, 0, m_strOriUrl, -1, buf, bufInlen, NULL, NULL);
	strcpy_s(buf, 100, (string("http://192.168.8.191/DVWA-master/")).c_str());
	_cprintf("%d,%d,%s,%d\n", m_totalNum, m_totalTestNum, buf, sizeof(buf));
	m_pData->setUrl(buf);

	Item *pItem = new Item(HttpMethod::get,"http://192.168.8.191/DVWA-master/vulnerabilities/sqli_blind/");
	vector<Field> args;
	Field field("id", "");
	args.push_back(field);
	Field field2("Submit", "Submit");
	args.push_back(field2);
	
	pItem->setArgs(args);
	string str = "Set-Cookie: PHPSESSID=6be1gvqmug35a7qc0tau0tn007; security=low";
	m_pData->analyseHeader(str);

	string headerStr;
	CHttpClient *pHttp = new CHttpClient();
	pHttp->setHeaderOpt("", headerStr);
	pSQLiTest->test(pHttp, pItem);


	_cprintf("%s\n used Time:%d\n", pSQLiTest->resultToString().c_str(), (clock() - start) / CLOCKS_PER_SEC);
	WriteFile("��ַ���������Խ��.csv", pSQLiTest->resultToStringForCSV());*/


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
			m_pData->setUrl(buf);
		//	m_pThreadPool = new CMyThreadPool(8);
		//	pSQLiTest = new CSQLiTest(m_pData);

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

void CMainPageDlg::setGlobalData(CData *pData, CMyThreadPool *pThreadPool, CSQLiTest* pSQLiTestG)
{
	m_pData = pData;
	m_pThreadPool = pThreadPool;
	pSQLiTest = pSQLiTestG;
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