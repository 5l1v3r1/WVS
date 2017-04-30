
// WVSDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


#include "ConfigDlg.h"
#include "MainPageDlg.h"
#include "TestManager.h"

// CWVSDlg 对话框
class CWVSDlg : public CDialogEx
{
// 构造
public:
	CWVSDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WVS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
	CConfigDlg m_config;
	CMainPageDlg m_mainPage;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);

public:

	CData *m_pData;
	CMyThreadPool *m_pThreadPool;
//	CSQLiTest* pSQLiTest;
	TestManager *m_pTestManager;
	
};
