// ConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WVS.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"


// CConfigDlg �Ի���

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigDlg::IDD, pParent)
	, m_crawlerLayer(3)
	, m_numOfThread(8)
	, m_isUseProxy(TRUE)
	, m_proxy(_T("http://127.0.0.1:8888"))
	, m_useErrorBased(FALSE)
	, m_useBoolBased(FALSE)
	, m_useTimeBased(FALSE)
	, m_testUrl(_T("http://192.168.8.191/DVWA-master/vulnerabilities/sqli_blind/"))
	, m_testArgs(_T(""))
	, m_testCookie(_T("security=low; PHPSESSID=r91ckd08o4mc3q1aci621f11k4"))
	, m_testArgName(_T(""))
	, m_testArgValue(_T(""))
	, m_methodRadio(0)
	, m_testSQLi(TRUE)
	, m_testXSS(TRUE)
{
	pTestItem = new Item();
	pTestArgs = new vector<Field>();
	CHttpClient::s_useProxy = m_isUseProxy;
	CHttpClient::s_proxy = CStrToStr(m_proxy);
}

CConfigDlg::~CConfigDlg()
{

}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LAYER_EDIT, m_crawlerLayer);
	DDX_Text(pDX, IDC_NUM_OF_THREAD_EDIT, m_numOfThread);
	DDV_MinMaxUInt(pDX, m_numOfThread, 2, 20);
	DDX_Check(pDX, IDC_USE_PROXY_CHECK, m_isUseProxy);
	DDX_Text(pDX, IDC_PROXY_EDIT, m_proxy);
	DDX_Check(pDX, IDC_ERROR_BASED_CHECK, m_useErrorBased);
	DDX_Check(pDX, IDC_BOOL_BASED_CHECK, m_useBoolBased);
	DDX_Check(pDX, IDC_TIME_BASED_CHECK, m_useTimeBased);
	DDX_Text(pDX, IDC_EDIT3, m_testUrl);
	DDX_Text(pDX, IDC_EDIT4, m_testArgs);
	DDX_Text(pDX, IDC_EDIT6, m_testCookie);
	DDX_Text(pDX, IDC_NAME_EDIT, m_testArgName);
	DDX_Text(pDX, IDC_VALUE_EDIT, m_testArgValue);
	DDX_Radio(pDX, IDC_RADIO1, m_methodRadio);
	DDX_Check(pDX, IDC_CHECK2, m_testSQLi);
	DDX_Check(pDX, IDC_CHECK3, m_testXSS);
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CConfigDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CConfigDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CConfigDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON5, &CConfigDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CConfigDlg::OnBnClickedButton4)
	ON_MESSAGE(WM_TEST_JOB, &CConfigDlg::OnTestJob)
END_MESSAGE_MAP()


// CConfigDlg ��Ϣ�������
void CConfigDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_pTestManager->loadConfiguration();
}

void CConfigDlg::setGlobalData(CData *pData, CMyThreadPool *pThreadPool, TestManager* pTestManager)
{
	m_pData = pData;
	m_pThreadPool = pThreadPool;
	m_pTestManager = pTestManager;
}


void CConfigDlg::OnBnClickedButton1()
{
	m_pTestManager->saveConfiguration();
}


//��������
void CConfigDlg::OnBnClickedButton3()
{
	UpdateData(TRUE);
	if (m_isUseProxy)
	{
		CHttpClient::s_useProxy = true;
		CHttpClient::s_proxy = CStrToStr(m_proxy);
	}
	else
	{
		CHttpClient::s_useProxy = false;
	}

	//m_pThreadPool->setThreadNum(m_numOfThread);	�ݲ�֧��
	m_pData->crawlerLayer = m_crawlerLayer;

	m_pTestManager->setTestMode((bool)m_useErrorBased, (bool)m_useBoolBased, (bool)m_useTimeBased);

}


//��ʼ����
void CConfigDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);

	HttpMethod method;
	if (m_methodRadio == 0)
	{
		method = HttpMethod::get;
	}
	else{
		method = HttpMethod::post;
	}

	string headerStr;
	pTestItem->setMethod(method);
	pTestItem->setUrl(CStrToStr(m_testUrl));
	vector<Field> args;
	Field field("id", "");
	args.push_back(field);
	Field field2("Submit", "Submit");
	args.push_back(field2);

	pTestItem->setArgs(args);

	//pTestItem->setArgs(*pTestArgs);
	string str = "Set-Cookie: " + CStrToStr(m_testCookie);//"Set-Cookie: PHPSESSID=6be1gvqmug35a7qc0tau0tn007; security=low";

	//pTestItem->setUrl("http://192.168.8.191/DVWA-master/vulnerabilities/sqli_blind/");
	//string str = "Set-Cookie: security=low; PHPSESSID=6be1gvqmug35a7qc0tau0tn007";
	m_pData->analyseHeader(str);
	TestJob *pJob = new TestJob(this->m_hWnd, pTestItem, m_pData, m_pTestManager);
	m_pThreadPool->addJob(pJob, NULL);
}

//��Ӳ���
void CConfigDlg::OnBnClickedButton5()
{
	UpdateData(TRUE);
	Field field(CStrToStr(m_testArgName), CStrToStr(m_testArgValue));
	if (!m_testSQLi)
	{
		//�ò���������SQLi����
		field.setSecurityFlag(1);
	}
	if (!m_testXSS)
	{
		field.setSecurityFlag(2);
	}
	pTestArgs->push_back(field);

	string showArgs = "";
	for (unsigned i = 0; i < pTestArgs->size(); i++)
	{
		showArgs += (*pTestArgs)[i].getName() + "=" + (*pTestArgs)[i].getValue() + "{"+ (m_testSQLi? "0":"1")+ "}";
		if (i < pTestArgs->size() - 1)
		{
			showArgs += "&";
		}
	}
	m_testArgs = StrToCStr(showArgs);
	m_testArgValue = "";
	m_testArgName = "";
	
	UpdateData(FALSE);
}


void CConfigDlg::OnBnClickedButton4()
{
	pTestArgs->clear();
	m_testArgs = "";
	UpdateData(FALSE);
}

LRESULT CConfigDlg::OnTestJob(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case 0:
			_cprintf("%s\n used Time:%d\n", m_pTestManager->resultToString().c_str(), (clock() - lParam) / CLOCKS_PER_SEC);
			WriteFile("��ַ���������Խ��.csv", m_pTestManager->resultToStringForCSV());
			break;
		default:
			_cprintf(" CConfigDlg::OnTestJob(WPARAM wParam, LPARAM lParam)   unknown command%d", wParam);
	}
	return 0;
}

