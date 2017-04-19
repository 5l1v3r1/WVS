#pragma once
#include "afxcmn.h"
#include "ScheduleJob.h"
#include "CThreadPool.h"

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
	CTreeCtrl m_urlTree;
	// 网址总数
	UINT m_totalNum;
	// 已爬行的网址数，由于格式为int(%int)，所以写成string型。
	CString m_crawledNum;
	// 总的测试项，每一个参数为单位1。
	UINT m_totalTestNum;
	afx_msg void OnBnClickedBegin();
	// 网站地址
	CString m_strOriUrl;

	CButton m_btnBegin;

	CScheduleJob* m_pSchedule;
	CThreadManage* m_pManage;
};
