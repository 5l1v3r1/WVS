#pragma once
#include "afxcmn.h"
#include"curl\curl.h"
#include "MyThreadPool.h"
#include "Data.h"
#include "ExtractJob.h"
#include "MonitorJob.h"
#include "SQLiTest.h"
#include "TestJob.h"
#include "TestManager.h"



using namespace std;



// CMainPageDlg �Ի���

class CMainPageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainPageDlg)

public:
	CMainPageDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMainPageDlg();


	void setGlobalData(CData *pData, CMyThreadPool *pThreadPool, TestManager* pTestManager);


// �Ի�������
	enum { IDD = IDD_MAINPAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeUrlEdit();
	afx_msg void OnBnClickedBegin();
	afx_msg LRESULT OnMONITOR(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);

	void updateTree();
	HTREEITEM FindItem(const CString& name, CTreeCtrl& tree, HTREEITEM hRoot);


private:
	CTreeCtrl m_urlTree;
	UINT m_totalNum; 	// ��ַ����
	CString m_crawledNum;// �����е���ַ�������ڸ�ʽΪint(%int)������д��string�͡�
	UINT m_totalTestNum; // �ܵĲ����ÿһ������Ϊ��λ1��
	CString m_strOriUrl; // ��վ��ַ
	CButton m_btnBegin;
	BOOL firstFlag = TRUE;

	CData *m_pData;
	CMyThreadPool *m_pThreadPool;
	TestManager* m_pTestManager;

	clock_t start, end;
	UINT m_usedTime;
	int m_workState = 1; //1����һ�ο�ʼǰ�� -1�������У��ǵ�һ��, 2:��ͣ


	HTREEITEM m_hRoot = NULL;
	
	
};
