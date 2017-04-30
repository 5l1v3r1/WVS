#pragma once
#include "Data.h"
#include "MyThreadPool.h"
#include "SQLiTest.h"
#include "Item.h"
#include "TestJob.h"
#include "TestManager.h"

// CConfigDlg �Ի���

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigDlg();

	void setGlobalData(CData *pData, CMyThreadPool *pThreadPool, TestManager* pTestManager);



// �Ի�������
	enum { IDD = IDD_CONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();		//���ز�������
	afx_msg void OnBnClickedButton1();	//�洢��������
	afx_msg void OnBnClickedButton3();	//load configuration  ���а������ò������ͣ����Դӽ�������չ������ʹ�������ļ�
	afx_msg void OnBnClickedButton2();	//��ʼ��������
	afx_msg LRESULT OnTestJob(WPARAM wParam, LPARAM lParam);	//����������ɵ�֪ͨ��Ϣ������
	afx_msg void OnBnClickedButton5();	//Ϊ����������Ӳ���
	afx_msg void OnBnClickedButton4();  //Ϊ����������ղ���

	CData *m_pData;
	CMyThreadPool *m_pThreadPool;
	TestManager* m_pTestManager;
	
	
	UINT m_crawlerLayer;// �������
	UINT m_numOfThread;
	BOOL m_isUseProxy;
	CString m_proxy;
	BOOL m_useErrorBased = TRUE;
	BOOL m_useBoolBased = TRUE;
	BOOL m_useTimeBased = TRUE;

	
	//һ�²�����Ϊ���Ե���url׼��
	CString m_testUrl;// ���Ե�����ַ��url
	CString m_testArgs; // ���Ե�����ַ�Ĳ���
	CString m_testCookie;// ���Ե�����ַ��cookie
	Item *pTestItem;
	vector<Field>*pTestArgs;
	CString m_testArgName;
	CString m_testArgValue;	
	BOOL m_methodRadio; 	
	BOOL m_testSQLi;// �ò����Ƿ����XSS����
	BOOL m_testXSS;// �ò����Ƿ����xss����
};
