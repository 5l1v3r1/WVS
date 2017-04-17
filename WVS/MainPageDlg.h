#pragma once
#include "afxcmn.h"

#include"curl\curl.h"
#include<iostream>
#include<string>
#include<fstream>
#include <queue>
#include <regex>
#include <list>

#include"CHttpClient.h"
#include "CThreadPool.h"
#include "HtmlForm.h"
#include "Item.h"

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
	CTreeCtrl m_urlTree;
	// ��ַ����
	UINT m_totalNum;
	// �����е���ַ�������ڸ�ʽΪint(%int)������д��string�͡�
	CString m_crawledNum;
	// �ܵĲ����ÿһ������Ϊ��λ1��
	UINT m_totalTestNum;

	afx_msg void OnBnClickedBegin();
	// ��վ��ַ
	CString m_strOriUrl;
};
