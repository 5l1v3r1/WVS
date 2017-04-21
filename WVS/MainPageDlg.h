#pragma once
#include "afxcmn.h"

#include "MyThreadPool.h"
#include "Data.h"
#include "ExtractJob.h"
#include "MonitorJob.h"

using namespace std;



// CMainPageDlg �Ի���

class CMainPageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainPageDlg)

public:
	CMainPageDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMainPageDlg();

// �Ի�������
	enum { IDD = IDD_MAINPAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeUrlEdit();
	afx_msg void OnEnChangeNetaddress1();
	afx_msg void OnBnClickedBegin();
	afx_msg LRESULT OnMONITOR(WPARAM wParam, LPARAM lParam);
	CTreeCtrl m_urlTree;

	UINT m_totalNum; 	// ��ַ����
	CString m_crawledNum;// �����е���ַ�������ڸ�ʽΪint(%int)������д��string�͡�
	UINT m_totalTestNum; // �ܵĲ����ÿһ������Ϊ��λ1��
	CString m_strOriUrl; // ��վ��ַ
	CButton m_btnBegin;
	BOOL firstFlag = TRUE;
	CData *m_pData;
	CMyThreadPool *pThreadPool;
	clock_t start, end;

};
