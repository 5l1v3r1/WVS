
// WVSDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


#include "ConfigDlg.h"
#include "MainPageDlg.h"
#include "TestManager.h"

// CWVSDlg �Ի���
class CWVSDlg : public CDialogEx
{
// ����
public:
	CWVSDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WVS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
