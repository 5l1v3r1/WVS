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
#include "ConfigDlg.h"
using namespace std;
// CMainPageDlg 对话框
class ResultDialog;

class CMainPageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainPageDlg)

public:
	CMainPageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainPageDlg();

	void updateTree();
	HTREEITEM FindItem(const CString& name, CTreeCtrl& tree, HTREEITEM hRoot);
	void setGlobalData(CData *pData, CMyThreadPool *pThreadPool, TestManager* pTestManager, CConfigDlg *pConfigDlg);
	void initBeforeCheck();
	afx_msg void OnEnChangeUrlEdit();
	afx_msg void OnBnClickedBegin();
	afx_msg LRESULT OnMONITOR(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedShowResultButton();

	enum { IDD = IDD_MAINPAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

private:
	CTreeCtrl m_urlTree;
	UINT m_totalNum; 	// 网址总数
	UINT m_totalTestNum; // 总的测试项，每一个参数为单位1。
	CString m_strOriUrl; // 网站地址
	CButton m_btnBegin;
	CConfigDlg* m_pConfigDialog;

	CData *m_pData;
	CMyThreadPool *m_pThreadPool;
	TestManager* m_pTestManager;

	clock_t start, end;
	UINT m_usedTime;
	int m_workState = 1; //1：第一次开始前， -1，工作中，非第一次, 2:暂停
	CString m_cstrResult = L"";	//存储结果
	HTREEITEM m_hRoot = NULL;
	int m_testItemNum;
	int m_extractedLinkNum;

	ResultDialog *m_pResultDialog = NULL ;	//显示结果的对话框
};
