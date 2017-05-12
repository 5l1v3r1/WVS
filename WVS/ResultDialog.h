#pragma once
#include "resource.h"

// ResultDialog 对话框

class ResultDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ResultDialog)

public:
	ResultDialog(CWnd* pParent = NULL, CString result = L"");   // 标准构造函数
	virtual ~ResultDialog();

// 对话框数据
	enum { IDD = IDD_RESULT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_result;
};
