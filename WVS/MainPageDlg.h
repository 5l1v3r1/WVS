#pragma once
#include "afxcmn.h"

#include "MyThreadPool.h"
#include "Data.h"
#include "ExtractJob.h"
#include "MonitorJob.h"

using namespace std;



// CMainPageDlg 对话框

class CMainPageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainPageDlg)

public:
	CMainPageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainPageDlg();

// 对话框数据
	enum { IDD = IDD_MAINPAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeUrlEdit();
	afx_msg void OnEnChangeNetaddress1();
	afx_msg void OnBnClickedBegin();
	afx_msg LRESULT OnMONITOR(WPARAM wParam, LPARAM lParam);
	CTreeCtrl m_urlTree;

	UINT m_totalNum; 	// 网址总数
	CString m_crawledNum;// 已爬行的网址数，由于格式为int(%int)，所以写成string型。
	UINT m_totalTestNum; // 总的测试项，每一个参数为单位1。
	CString m_strOriUrl; // 网站地址
	CButton m_btnBegin;
	BOOL firstFlag = TRUE;
	CData *m_pData;
	CMyThreadPool *pThreadPool;
	clock_t start, end;

};
