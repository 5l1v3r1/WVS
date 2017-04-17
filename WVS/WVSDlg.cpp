
// WVSDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WVS.h"
#include "WVSDlg.h"
#include "afxdialogex.h"

#include "conio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CWVSDlg 对话框



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
	ON_WM_SIZE()	//添加：
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CWVSDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CWVSDlg 消息处理程序

BOOL CWVSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	

	// TODO:  在此添加额外的初始化代码
	CRect tabRect;
	
	m_tab.InsertItem(0, _T("主页"));
	m_tab.InsertItem(1, _T("配置"));
	m_mainPage.Create(IDD_MAINPAGE_DIALOG, &m_tab);
	m_config.Create(IDD_CONFIG_DIALOG, &m_tab);
	m_tab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 25;
	tabRect.bottom -= 1;
	m_mainPage.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
	m_config.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWVSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		//m_tab.SetWindowPos(NULL, rect.left - 1, rect.top + 10, rect.Width() - 2, rect.Height() - 26, SWP_SHOWWINDOW);
		// 绘制图标
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWVSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWVSDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	if (pResult != NULL)
		*pResult = 0;
	/*CWnd * phMainPageWnd = m_tab.GetDlgItem(IDD_MAINPAGE_DIALOG);		//为什么这里取出来的是空？？
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
