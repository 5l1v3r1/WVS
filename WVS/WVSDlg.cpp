
// WVSDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WVS.h"
#include "WVSDlg.h"
#include "afxdialogex.h"

#include "conio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWVSDlg �Ի���



CWVSDlg::CWVSDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWVSDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWVSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CWVSDlg, CDialogEx)
	ON_WM_SIZE()	//��ӣ�
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CWVSDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CWVSDlg ��Ϣ�������

BOOL CWVSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	CRect tabRect;
	
	m_tab.InsertItem(0, _T("��ҳ"));
	m_tab.InsertItem(1, _T("����"));
	m_mainPage.Create(IDD_MAINPAGE_DIALOG, &m_tab);
	m_config.Create(IDD_CONFIG_DIALOG, &m_tab);
	m_tab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 25;
	tabRect.bottom -= 1;
	m_mainPage.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
	m_config.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CWVSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWVSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		//m_tab.SetWindowPos(NULL, rect.left - 1, rect.top + 10, rect.Width() - 2, rect.Height() - 26, SWP_SHOWWINDOW);
		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}



void CWVSDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect rectWnd;
	CRect wnd;
	//this->GetWindowRect(&rectWnd);
	CWnd * phWnd = GetDlgItem(IDC_TAB1);
	this->GetWindowRect(&wnd);
	if (phWnd != NULL){
		
		phWnd->GetWindowRect(&rectWnd);
		//ScreenToClient(rectWnd);
		//CManageTabDlg::GetNewRect(Rect, m_Multiple_width, m_Mutiple_heith, bSizeFlag);
		//phWnd->MoveWindow(rectWnd, TRUE);
		//phWnd->SetWindowPos(NULL, rectWnd.left, rectWnd.top, rectWnd.Width(), rectWnd.Height(), SWP_SHOWWINDOW);
		phWnd->SetWindowPos(NULL, 10, 12, cx-24, cy - 25, SWP_SHOWWINDOW);
		OnTcnSelchangeTab1(NULL, NULL);
	}

	AllocConsole();
//	_cprintf("%d, %d, (%d, %d, %d, %d)\t(%d,%d,%d,%d)\n", cx, cy, rectWnd.left,rectWnd.top, rectWnd.Width(),rectWnd.Height(), wnd.left, wnd.top, wnd.Width(), wnd.Height());
	
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CWVSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWVSDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (pResult != NULL)
		*pResult = 0;
	/*CWnd * phMainPageWnd = m_tab.GetDlgItem(IDD_MAINPAGE_DIALOG);		//Ϊʲô����ȡ�������ǿգ���
	CWnd * phConfigWnd = m_tab.GetDlgItem(IDD_CONFIG_DIALOG);
	if (phMainPageWnd != NULL && phConfigWnd != NULL)
	{

	}
	else{
	_cprintf("%s\n", "all null");
	}
	*/
	CRect tabRect;
	m_tab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 25;
	tabRect.bottom -= 1;
	switch (m_tab.GetCurSel())
	{
		case 0:
			m_mainPage.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			m_config.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);

			break;
		case 1:
			m_config.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			m_mainPage.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
			break;
		default:
			break;
	}

}
