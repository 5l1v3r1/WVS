
// WVS.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
//#pragma depress(warning:LNK4099)


// CWVSApp: 
// �йش����ʵ�֣������ WVS.cpp
//

class CWVSApp : public CWinApp
{
public:
	CWVSApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWVSApp theApp;