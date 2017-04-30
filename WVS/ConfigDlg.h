#pragma once
#include "Data.h"
#include "MyThreadPool.h"
#include "SQLiTest.h"
#include "Item.h"
#include "TestJob.h"

// CConfigDlg 对话框

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigDlg();

	void setGlobalData(CData *pData, CMyThreadPool *pThreadPool, CSQLiTest* pSQLiTest);



// 对话框数据
	enum { IDD = IDD_CONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	CData *m_pData;
	CMyThreadPool *m_pThreadPool;
	CSQLiTest* pSQLiTest;
	afx_msg void OnBnClickedButton1();
	// 爬行深度
	UINT g_crawlerLayer;
	UINT m_numOfThread;
	BOOL m_isUseProxy;
	CString m_proxy;
	BOOL m_useErrorBased = TRUE;
	BOOL m_useBoolBased = TRUE;
	BOOL m_useTimeBased = TRUE;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg LRESULT OnTestJob(WPARAM wParam, LPARAM lParam);
	// 测试单个网址的url
	CString m_testUrl;
	// 测试单个网址的参数
	CString m_testArgs;
	// 测试单个网址的cookie
	CString m_testCookie;
	Item *pTestItem;
	vector<Field>*pTestArgs;
	afx_msg void OnBnClickedButton5();
	CString m_testArgName;
	CString m_testArgValue;
	afx_msg void OnBnClickedButton4();
	BOOL m_methodRadio;
	// 该参数是否进行xss测试
	BOOL m_testSQLi;
	// 该参数是否进行XSS测试
	BOOL m_testXSS;
};
