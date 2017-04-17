#pragma once


// CManageTabDlg 对话框

class CManageTabDlg : public CTabCtrl
{
	DECLARE_DYNAMIC(CManageTabDlg)

public:
	CManageTabDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManageTabDlg();

// 对话框数据
	enum { IDD = IDD_WVS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
