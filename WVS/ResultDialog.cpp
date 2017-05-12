// ResultDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "WVS.h"
#include "ResultDialog.h"
#include "afxdialogex.h"


// ResultDialog 对话框

IMPLEMENT_DYNAMIC(ResultDialog, CDialogEx)

ResultDialog::ResultDialog(CWnd* pParent /*= NULL*/, CString result /*= L""*/) : CDialogEx(ResultDialog::IDD, pParent)
	, m_result(_T(""))
{
	m_result = result;
}

ResultDialog::~ResultDialog()
{

}

void ResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RESULT_EDIT, m_result);
}


BEGIN_MESSAGE_MAP(ResultDialog, CDialogEx)
END_MESSAGE_MAP()


// ResultDialog 消息处理程序
