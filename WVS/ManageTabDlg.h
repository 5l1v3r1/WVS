#pragma once


// CManageTabDlg �Ի���

class CManageTabDlg : public CTabCtrl
{
	DECLARE_DYNAMIC(CManageTabDlg)

public:
	CManageTabDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManageTabDlg();

// �Ի�������
	enum { IDD = IDD_WVS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
