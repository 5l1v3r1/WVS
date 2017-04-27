// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WVS.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"


// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigDlg::IDD, pParent)
{

}

CConfigDlg::~CConfigDlg()
{

}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CConfigDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序


void CConfigDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	pSQLiTest->loadConfiguration();
}

void CConfigDlg::setGlobalData(CData *pData, CMyThreadPool *pThreadPool, CSQLiTest* pSQLiTestG)
{
	m_pData = pData;
	m_pThreadPool = pThreadPool;
	pSQLiTest = pSQLiTestG;
}


void CConfigDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	pSQLiTest->saveConfiguration();
}
