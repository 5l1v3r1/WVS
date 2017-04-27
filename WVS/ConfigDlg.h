#pragma once
#include "Data.h"
#include "MyThreadPool.h"
#include "SQLiTest.h"

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
};
