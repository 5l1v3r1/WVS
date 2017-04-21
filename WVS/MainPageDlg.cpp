// MainPageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WVS.h"
#include "MainPageDlg.h"
#include "afxdialogex.h"

// CMainPageDlg 对话框

IMPLEMENT_DYNAMIC(CMainPageDlg, CDialogEx)

CMainPageDlg::CMainPageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainPageDlg::IDD, pParent)
	, m_totalNum(0)
	, m_crawledNum(_T(""))
	, m_totalTestNum(0) 
	, m_strOriUrl(_T(""))
{
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
}


BEGIN_MESSAGE_MAP(CMainPageDlg, CDialogEx)
	
	ON_EN_CHANGE(IDC_NETADDRESS1, &CMainPageDlg::OnEnChangeNetaddress1)
	ON_BN_CLICKED(ID_BEGIN, &CMainPageDlg::OnBnClickedBegin)
	ON_MESSAGE(WM_MY_MONITOR, &CMainPageDlg::OnMONITOR)
END_MESSAGE_MAP()

// CMainPageDlg 消息处理程序



void CMainPageDlg::OnEnChangeNetaddress1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	//m_oriUrl.DisplayErrorTip();
}


LRESULT CMainPageDlg::OnMONITOR(WPARAM wParam, LPARAM lParam)
{
	CString cStr;
	switch (wParam){
		case 0:{
				   cStr.Format(L"总耗时%d 秒\n结束了", lParam);
					break;
		}
		case 1:{
				  
				   cStr.Format(L"耗时%d 秒", lParam);
		}
	}
	SetDlgItemText(ID_BEGIN, cStr);
	
	return NULL;
}

void CMainPageDlg::OnBnClickedBegin()
{
	// TODO:  在此添加控件通知处理程序代码
	CString cStr;
	GetDlgItemText(ID_BEGIN, cStr);
	if ( cStr== _T("开始"))
	{
		if (firstFlag)
		{
			start = clock();
			firstFlag = FALSE;
			//第一次，初始化
			UpdateData(TRUE);
			m_totalNum = 100;
			m_crawledNum = "50(%50)";
			m_totalTestNum = 200;
			const int bufInlen = 100;
			char buf[bufInlen];
			WideCharToMultiByte(CP_ACP, 0, m_strOriUrl, -1, buf, bufInlen, NULL, NULL);
			strcpy_s(buf, 100, (string("http://wangbaiyuan.cn")).c_str());
			_cprintf("%d,%d,%s,%d\n", m_totalNum, m_totalTestNum, buf, sizeof(buf));
			m_pData = new CData(buf);
			pThreadPool = new CMyThreadPool(8);
			Item *pItem = new Item(HttpMethod::get, buf);
			MonitorJob *pMJob = new MonitorJob(this->m_hWnd, start, pThreadPool);
			pThreadPool->addJob(pMJob, NULL);
			CExtractJob *pJob = new CExtractJob(pItem, m_pData);
			pThreadPool->addJob(pJob, NULL);
		}
		else{
			//继续执行任务。
			_cprintf("continue\n");
		}
		SetDlgItemText(ID_BEGIN, L"暂停");
	}
	else{
		//暂停。
		_cprintf("pause\n");
		SetDlgItemText(ID_BEGIN, L"开始");
	}
}

