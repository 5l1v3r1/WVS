#pragma once
#include "resource.h"

// ResultDialog �Ի���

class ResultDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ResultDialog)

public:
	ResultDialog(CWnd* pParent = NULL, CString result = L"");   // ��׼���캯��
	virtual ~ResultDialog();

// �Ի�������
	enum { IDD = IDD_RESULT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_result;
};
