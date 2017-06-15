
// WVSDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WVS.h"
#include "WVSDlg.h"
#include "afxdialogex.h"

#include "conio.h"
#include <fcntl.h>   
#include <io.h>
#include <iostream>

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


void cls(void)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 }; //���λ��  
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// ��ǰbuf�е�����cell��Ŀ  
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// ��������Ļ���ո�  
	if (!FillConsoleOutputCharacter(hConsole,
		TEXT(' '),
		dwConSize,
		coordScreen,
		&cCharsWritten))
	{
		return;
	}
	//��ȡ��������  
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// ����buf����  
	if (!FillConsoleOutputAttribute(hConsole,
		csbi.wAttributes,
		dwConSize,
		coordScreen,
		&cCharsWritten))
	{
		return;
	}
	// ������ƶ���ʼλ��  
	SetConsoleCursorPosition(hConsole, coordScreen);
}

// SetConsoleMode();  
BOOL SetConsoleMode(int cols, int lines)
{
	BOOL retval = FALSE;
	HANDLE handle = ::CreateFile(
		TEXT("CONOUT$"),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return retval;
	}

	// ��ȡԭʼ��С  
	CONSOLE_SCREEN_BUFFER_INFO OldConsoleScreenBufferInfo = { 0 };
	if (!GetConsoleScreenBufferInfo(handle, &OldConsoleScreenBufferInfo))
	{
		CloseHandle(handle);
		return retval;
	}

	// ��ȡԭ��buf����  
	DWORD dwsize = OldConsoleScreenBufferInfo.dwSize.X*OldConsoleScreenBufferInfo.dwCursorPosition.Y + OldConsoleScreenBufferInfo.dwCursorPosition.X;
	LPWSTR buf = new WCHAR[dwsize];
	COORD readpos = { 0, 0 };
	DWORD dwBytes = 0;
	ReadConsoleOutputCharacter(handle, buf, dwsize, readpos, &dwBytes);

	do
	{
		SMALL_RECT rect;
		rect.Left = 0;
		rect.Top = 0;
		rect.Right = cols - 1;
		rect.Bottom = lines - 1;
		SetConsoleWindowInfo(handle, TRUE, &rect);

		// ����buf  
		COORD dwnewPosition;
		dwnewPosition.X = cols;
		dwnewPosition.Y = lines;
		SetConsoleScreenBufferSize(handle, dwnewPosition);

		// �����´��ڴ�С  
		CONSOLE_SCREEN_BUFFER_INFO NewConsoleScreenBufferInfo = { 0 };
		if (!GetConsoleScreenBufferInfo(handle, &NewConsoleScreenBufferInfo))
			break;
		if (lines >= 80)
		{
			rect.Right = NewConsoleScreenBufferInfo.dwMaximumWindowSize.X - 1;
		}
		else
		{
			rect.Right = (lines << 1);
		}

		if (cols >= 25)
		{
			rect.Bottom = NewConsoleScreenBufferInfo.dwMaximumWindowSize.Y - 1;
		}
		else
		{
			rect.Bottom = (cols >> 1);
		}

		if (SetConsoleWindowInfo(handle, TRUE, &rect))
		{
			retval = TRUE;
		}
	} while (0);

	CloseHandle(handle);

	// д��ԭʼ����   
	DWORD curpos = 0;
	HANDLE outhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	cls();
	for (unsigned int i = 0; i < dwsize; i += OldConsoleScreenBufferInfo.dwSize.X)
	{
		BOOL newline = FALSE;
		if (dwsize - i < 79)
		{
			WriteFile(outhandle, buf + i, dwsize - i, &dwBytes, NULL);
			break;
		}
		DWORD dwwritebyte = 79;
		while (*(buf + i + dwwritebyte) == 0x20)
		{
			newline = TRUE;
			dwwritebyte--;
		}
		WriteFile(outhandle, buf + i, dwwritebyte + 1, &dwBytes, NULL);
		if (newline)
		{
			WriteFile(outhandle, "\r\n", 2, &dwBytes, NULL);
		}
	}
	delete[]buf;
	return retval;
}


void setConsole()
{
	AllocConsole();
	//Retrieves a handle to the specified standard device (standard input, standard output, or standard error).   
	HANDLE hin = ::GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hout = ::GetStdHandle(STD_OUTPUT_HANDLE);
	//Associates a C run-time file descriptor with an existing operating-system file handle.   
	int hcin = _open_osfhandle((intptr_t)hin, _O_TEXT);                 // ��ʱhcin �ͳ���һ��file descriptor ��   
	//      When a program opens a file, the operating system returns a corresponding file descriptor that the program refers to   
	//     in order to process the file. A file descriptor is a low positive integer. The first three file descriptors (0,1, and 2,)   
	//     are associated with the standard input (stdin), the standard output (stdout), and the standard error (stderr), respectively.   
	//     Thus, the function scanf() uses stdin and the function printf() uses stdout. You can override the default setting and   
	//     re-direct the process's I/O to different files by using different file descriptors:   
	//     #include <cstdio>   
	//     fprintf(stdout, "writing to stdout"); //write to stdout instead of a physical file   
	FILE * fpin = _fdopen(hcin, "r");
	*stdin = *fpin;                                                  //stdin ��ָ�����ļ�ָ��   
	int hcout = _open_osfhandle((intptr_t)hout, _O_TEXT);
	FILE * fpout = _fdopen(hcout, "wt");
	*stdout = *fpout;
	std::ios_base::sync_with_stdio();           // ��iostream ��ͬc runtime lib ��stdio ͬ������׼��ͬ����   
	//	_cprintf("%d, %d, (%d, %d, %d, %d)\t(%d,%d,%d,%d)\n", cx, cy, rectWnd.left,rectWnd.top, rectWnd.Width(),rectWnd.Height(), wnd.left, wnd.top, wnd.Width(), wnd.Height());
	SetConsoleMode(200, 60);
}


// CWVSDlg �Ի���
CWVSDlg::CWVSDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWVSDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_APP);
#ifdef DEBUG_MY
	setConsole();
#endif
	m_pData = new Data();
	m_pThreadPool = new	ThreadPool(-1);	
	m_pTestManager = new TestManager(m_pData);
	m_config.setGlobalData(m_pData, m_pThreadPool, m_pTestManager);
	m_mainPage.setGlobalData(m_pData, m_pThreadPool, m_pTestManager, &m_config);
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
