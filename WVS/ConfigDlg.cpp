// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WVS.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"
#include "Field.h"
// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigDlg::IDD, pParent)
	, m_crawlerLayer(3)
	, m_numOfThread(5)
	, m_isUseProxy(FALSE)
	, m_proxy(_T("http://127.0.0.1:8888"))
	, m_useErrorBased(TRUE)
	, m_useBoolBased(TRUE)
	, m_useTimeBased(FALSE)
	, m_testUrl(_T(""))
	, m_testArgs(_T(""))
	, m_testCookie(_T(""))
	, m_testArgName(_T(""))
	, m_testArgValue(_T(""))
	, m_methodRadio(0)
	, m_testSQLi(TRUE)
	, m_testXSS(TRUE)
	, m_useXSSTest(TRUE)
	, m_cstrUserName(_T(""))
	, m_cstrPassword(_T(""))
	, m_cstrExclude(_T(""))
{
	pTestItem = new Item();
	pTestArgs = new vector<Field>();
	CHttpClient::s_useProxy = (bool)m_isUseProxy;
	CHttpClient::s_proxy = CStrToStr(m_proxy);
}

CConfigDlg::~CConfigDlg()
{
	if (m_pResultDialog != NULL)
	{
		delete m_pResultDialog;
	}
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LAYER_EDIT, m_crawlerLayer);
	DDX_Text(pDX, IDC_NUM_OF_THREAD_EDIT, m_numOfThread);
	DDV_MinMaxUInt(pDX, m_numOfThread, 1, 30);
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
	DDX_Check(pDX, IDC_XSS_CHECK, m_useXSSTest);
	DDX_Control(pDX, IDC_BUTTON2, m_butStart);

	DDX_Text(pDX, IDC_EDIT7, m_cstrExclude);
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CConfigDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CConfigDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CConfigDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON5, &CConfigDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CConfigDlg::OnBnClickedButton4)
	ON_MESSAGE(WM_TEST_JOB, &CConfigDlg::OnTestJob)
	ON_BN_CLICKED(IDC_BUTTON6, &CConfigDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序
void CConfigDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	m_pTestManager->loadConfiguration();
}

void CConfigDlg::setGlobalData(Data *pData, ThreadPool *pThreadPool, TestManager* pTestManager)
{
	m_pData = pData;
	m_pThreadPool = pThreadPool;
	m_pTestManager = pTestManager;
}


void CConfigDlg::OnBnClickedButton1()
{
	m_pTestManager->saveConfiguration();
}


//保存配置
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

	m_pData->crawlerLayer = m_crawlerLayer;
	m_pData->userName = CStrToStr(m_cstrUserName);
	m_pData->password = CStrToStr(m_cstrPassword);
	/*Field::DEFAULT_NAME_VALUE = CStrToStr(m_cstrUserName);
	Field::DEFAULT_PASSWORD = CStrToStr(m_cstrPassword);*/
	m_pTestManager->setTestMode(m_useErrorBased, m_useBoolBased, m_useTimeBased, m_useXSSTest);
	m_pThreadPool->setThreadNum(m_numOfThread + 1);	//有一个线程作为监视线程
	if (m_testCookie.Trim() != "")
	{
		string str = "Set-Cookie: " + CStrToStr(m_testCookie);
		m_pData->analyseHeader(str);
	}
	if (m_cstrExclude.Trim() != "")
	{
		m_pData->excludeUrl.clear();
		split(CStrToStr(m_cstrExclude), string("&"), &(m_pData->excludeUrl));
	}

	string fileName = "网址\\" + to_string(time(NULL));
	system(("mkdir " + fileName).c_str());
	system(("move 网址* " + fileName).c_str());
	WriteLog("------------------------------------------新的一次扫描" + to_string(time(NULL)) + "-----------------------------------------------------");

}


//开始测试
void CConfigDlg::OnBnClickedButton2()
{
	OnBnClickedButton3();//初始化测试环境
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
	
	//测试用
	//vector<Field> args;
	//Field field("txtName", "");
	////field.setSecurityFlag(2);
	//args.push_back(field);
	//Field field2("btnSign", "Sign Guestbook");
	////field.setSecurityFlag(2);
	//args.push_back(field2);
	//Field field3("mtxMessage", "");
	////field.setSecurityFlag(2);
	//args.push_back(field3);
	//pTestItem->setArgs(args);
	//string str = string("Set-Cookie: ") + "security=low; PHPSESSID=rk50b0hvkdtjc3kbea37ilq9r6";        //"Set-Cookie: PHPSESSID=6be1gvqmug35a7qc0tau0tn007; security=low";
	//pTestItem->setUrl("http://192.168.152.128/DVWA-master/vulnerabilities/xss_s/");
	//以上测试用

	pTestItem->setArgs(*pTestArgs);
	string str = "Set-Cookie: " + CStrToStr(m_testCookie);
	string url = CStrToStr(m_testUrl);
	if (url.find("http") != 0)
	{
		url= "http://" + url;
	}
	pTestItem->setUrl(url);

	m_pData->analyseHeader(str);
	TestJob *pJob = new TestJob(this->m_hWnd, pTestItem, m_pData, m_pTestManager);
	m_pThreadPool->addJob(pJob, NULL);
	m_testWorkStatus = -1;
	SetDlgItemText(ID_BEGIN, L"测试中");
	m_butStart.EnableWindow(0);
}

//添加参数
void CConfigDlg::OnBnClickedButton5()
{
	UpdateData(TRUE);
	Field field(CStrToStr(m_testArgName), CStrToStr(m_testArgValue));
	if (!m_testSQLi)
	{
		//该参数不进行SQLi测试
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
		showArgs += (*pTestArgs)[i].getName() + "=" + (*pTestArgs)[i].getValue() + "{" + to_string((*pTestArgs)[i].getSecurityFlag()) + "}";
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
			WriteFile("网址树——测试结果.csv", m_pTestManager->resultToStringForCSV());
			m_testWorkStatus = 0;
			m_testCstrResult = StrToCStr(m_pTestManager->resultToStringFormat());
			OnBnClickedButton6();
			m_butStart.EnableWindow(1);
			break;
		default:
			_cprintf(" CConfigDlg::OnTestJob(WPARAM wParam, LPARAM lParam)   unknown command%d", wParam);
	}
	return 0;
}

void CConfigDlg::OnBnClickedButton6()
{
	if (m_testWorkStatus == 0)
	{
		if (m_pResultDialog == NULL)
		{
			m_pResultDialog = new ResultDialog(NULL, m_testCstrResult);
			m_pResultDialog->Create(IDD_RESULT_DIALOG); //创建一个非模态对话框  
		}
		else{
			m_pResultDialog->m_result = m_testCstrResult;
			m_pResultDialog->UpdateData(FALSE);
		}
		m_pResultDialog->ShowWindow(SW_SHOWNORMAL); //显示非模态对话框 
	}
	else
	{
		AfxMessageBox(L"检测尚未结束，请稍等....");
	}
}
