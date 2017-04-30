#pragma once
#include "Data.h"
#include "MyThreadPool.h"
#include "SQLiTest.h"
#include "Item.h"
#include "TestJob.h"

// CConfigDlg �Ի���

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigDlg();

	void setGlobalData(CData *pData, CMyThreadPool *pThreadPool, CSQLiTest* pSQLiTest);



// �Ի�������
	enum { IDD = IDD_CONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	CData *m_pData;
	CMyThreadPool *m_pThreadPool;
	CSQLiTest* pSQLiTest;
	afx_msg void OnBnClickedButton1();
	// �������
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
	// ���Ե�����ַ��url
	CString m_testUrl;
	// ���Ե�����ַ�Ĳ���
	CString m_testArgs;
	// ���Ե�����ַ��cookie
	CString m_testCookie;
	Item *pTestItem;
	vector<Field>*pTestArgs;
	afx_msg void OnBnClickedButton5();
	CString m_testArgName;
	CString m_testArgValue;
	afx_msg void OnBnClickedButton4();
	BOOL m_methodRadio;
	// �ò����Ƿ����xss����
	BOOL m_testSQLi;
	// �ò����Ƿ����XSS����
	BOOL m_testXSS;
};
