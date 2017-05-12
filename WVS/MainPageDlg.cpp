// MainPageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WVS.h"
#include "MainPageDlg.h"
#include "afxdialogex.h"
#include "ResultDialog.h"

// CMainPageDlg �Ի���

IMPLEMENT_DYNAMIC(CMainPageDlg, CDialogEx)


CMainPageDlg::CMainPageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainPageDlg::IDD, pParent)
	, m_totalNum(0)
	, m_totalTestNum(0) 
	, m_strOriUrl(_T(""))
	, m_usedTime(0)
	, m_testItemNum(0)
	, m_extractedLinkNum(0)
{
	
}

CMainPageDlg::~CMainPageDlg()
{

}

void CMainPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_urlTree);
	DDX_Text(pDX, IDC_TotalNum_EDIT, m_totalNum);
	DDX_Text(pDX, IDC_TotalTestNum_EDIT, m_totalTestNum);
	DDX_Text(pDX, IDC_EDIT1, m_strOriUrl);
	DDX_Control(pDX, ID_BEGIN, m_btnBegin);
	DDX_Text(pDX, IDC_UsedTime_EDIT, m_usedTime);
	DDX_Text(pDX, IDC_TotalTestNum_EDIT2, m_testItemNum);
	DDX_Text(pDX, IDC_CrawledNum_EDIT, m_extractedLinkNum);
}


BEGIN_MESSAGE_MAP(CMainPageDlg, CDialogEx)
	
	ON_BN_CLICKED(ID_BEGIN, &CMainPageDlg::OnBnClickedBegin)
	ON_MESSAGE(WM_MY_MONITOR, &CMainPageDlg::OnMONITOR)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CMainPageDlg::OnNMDblclkTree1)
	ON_BN_CLICKED(IDC_SHOW_RESULT_BUTTON, &CMainPageDlg::OnBnClickedShowResultButton)
END_MESSAGE_MAP()

LRESULT CMainPageDlg::OnMONITOR(WPARAM wParam, LPARAM lParam)
{
	CString cStr;
	switch (wParam){
		case 0:{
				   SetDlgItemText(ID_BEGIN, L"�ѽ���");
				   m_workState = 0;	//�����ˡ�
				  // m_pThreadPool->stop();
				   _cprintf("%s\n used Time:%d\n", m_pTestManager->resultToString().c_str(), (clock() - start) / CLOCKS_PER_SEC);
				   WriteFile("��ַ���������Խ��.csv", m_pTestManager->resultToStringForCSV());
				   m_cstrResult = StrToCStr(m_pTestManager->resultToStringFormat());
				   OnBnClickedShowResultButton();
				break;
		}
		case 1:{
				   
				   break;
		}
		case 2:{
				   //����һ���µ�©����������ͼ��
				   _cprintf("url:  %s\n", ((string*)lParam)->c_str());
				   break;
		}
		default:{
					WriteLog("monitor other command:" + to_string(wParam));
		}
	}
	
	m_usedTime = (clock() - start) / CLOCKS_PER_SEC;
	m_totalNum = m_pData->crawlerLinksItemVec.size();
	m_totalTestNum = m_pData->getTotalArgNum();
	m_extractedLinkNum = m_pData->getExtractLinkNum();
	m_testItemNum = m_pData->getTestedArgNum();
	updateTree();
	UpdateData(FALSE);
	return NULL;
}


void CMainPageDlg::OnBnClickedBegin()
{

	//testState: -2,   normalState =1;
	if (m_workState > 0)	//�ָ����߿�ʼ
	{	
		if (m_workState == 1)	//��ʼ
		{
			initBeforeCheck();
			start = clock();
			//��һ�Σ���ʼ��
			Item *pItem = new Item(HttpMethod::get, m_pData->originUrl);
			pItem->setLayer(0);
			m_pData->putItem(pItem);

			MonitorJob *pMJob = new MonitorJob(this->m_hWnd, start, m_pThreadPool);
			m_pThreadPool->addJob(pMJob, NULL);
			CExtractJob *pJob = new CExtractJob(pItem, m_pData, m_pTestManager, m_hWnd);
			m_pThreadPool->addJob(pJob, NULL);
		}
		else{
			//����ִ������
			_cprintf("continue\n");
			m_pThreadPool->resume();
			SetDlgItemText(ID_BEGIN, L"�����ͣ");
		}
	}
	else if(m_workState == -1){
		
		_cprintf("pause\n");
		m_pThreadPool->pause();
		SetDlgItemText(ID_BEGIN, L"�����ʼ");
		m_workState = 2;	//��ͣ��
	}
	else if(m_workState == 0){
		AfxMessageBox(L"�Ѿ������ˣ�");
	}
	else{
		CString cstr;
		cstr.Format(L"�쳣״̬!%d", m_workState);
		AfxMessageBox(cstr);
	}
}

void CMainPageDlg::setGlobalData(CData *pData, CMyThreadPool *pThreadPool, TestManager* pTestManager, CConfigDlg *pConfigDlg)
{
	m_pData = pData;
	m_pThreadPool = pThreadPool;
	m_pTestManager = pTestManager;
	m_pConfigDialog = pConfigDlg;
}

void CMainPageDlg::updateTree()
{
	if (m_hRoot == NULL)
	{
		m_hRoot = m_urlTree.InsertItem(StrToCStr(m_pData->domain), 0, 0);
	}
	static string delim = "/";
	static unsigned currentNum = 0;
	Item *pItem = m_pData->getItemByIndex(currentNum);
	bool  isEndWithDelim = FALSE;
	HTREEITEM hTemp = m_hRoot;
	CString tempCstr;
	bool isSameNodeExisted = false;
	vector<string> vecStr;
	string path;
	int posOfBegin = 0;
	while (pItem != NULL)
	{
		string url = pItem->getUrl();
		posOfBegin = url.find("/", 8);
		if (posOfBegin != -1)
		{
			path = url.substr(posOfBegin);
			if (path[path.size() - 1] == '/')
			{
				isEndWithDelim = true;
				path = path.substr(0, path.size() - 1);
			}
			split(path, delim, &vecStr);					//��/��ʼ���ַ������ص����ݵ�һ��Ϊ�մ�
			if (isEndWithDelim)
			{
				vecStr.push_back("/");
			}
			for (unsigned i = 1; i < vecStr.size(); i++)	//ȥ����/��ʼ��path�ĵ�һ��Ϊ�մ���Ӱ��
			{
				isSameNodeExisted = false;
				tempCstr = StrToCStr(vecStr[i]);
				HTREEITEM hSub = m_urlTree.GetChildItem(hTemp);
				while (hSub)
				{
					// check the children of the current item  
					CString text = m_urlTree.GetItemText(hSub);
					if (text.Compare(tempCstr) == 0)
					{
						hTemp = hSub;
						isSameNodeExisted = true;
						break;
					}
					else //��Ϊ�����������ֱ�ӱȽϴ�С������Ѿ��������ϵģ���ֱ�ӷ���δ�ҵ���
					{
						hSub = m_urlTree.GetNextSiblingItem(hSub);
					}
				}
				if (!isSameNodeExisted)
				{
					hTemp = m_urlTree.InsertItem(tempCstr, 0, 0, hTemp, TVI_SORT);
				}
				if (i == vecStr.size() - 1)
				{
					//���һ�Σ���Ҫ���������ý�ȥ��
					m_urlTree.SetItemData(hTemp, pItem->getId());
				}
			}
		}
		pItem = m_pData->getItemByIndex(++currentNum);
		hTemp = m_hRoot;
		isEndWithDelim = false;
		vecStr.clear();
		path = "";
		url = "";
	}
}




void CMainPageDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	HTREEITEM hItem = m_urlTree.GetSelectedItem();
	
	
	if (!m_urlTree.ItemHasChildren(hItem))
	{
		CString text = m_urlTree.GetItemText(hItem);
		//str.Format(L"%lu", pNMTreeView->itemNew.lParam);
		//AfxMessageBox(pNMTreeView->itemNew.pszText + str);
		unsigned id = m_urlTree.GetItemData(hItem);
		Item*pItem = m_pData->getItemByIndex(id);	//ʵ��������Ӧ����byId�����Ǵ�ʱ��index��id��ͬ���ʲ���дһ������
	//	_cprintf("Double Click to show info %s, id:%lu, %s\n", CStrToStr(text).c_str(), id, pItem->getArgsStr().c_str());
	}
	else{
		_cprintf("not the leaf node!");
	}
}

void CMainPageDlg::OnBnClickedShowResultButton()
{
	if (m_workState == 0)
	{
		if (m_pResultDialog == NULL)
		{
			m_pResultDialog = new ResultDialog(NULL, m_cstrResult);
			m_pResultDialog->Create(IDD_RESULT_DIALOG); //����һ����ģ̬�Ի���  
		}
		else{
			m_pResultDialog->m_result = m_cstrResult;
			m_pResultDialog->UpdateData(FALSE);
		}
		m_pResultDialog->ShowWindow(SW_SHOWNORMAL); //��ʾ��ģ̬�Ի��� 
	}
	else
	{
		AfxMessageBox(L"�����δ���������Ե�....");
	}
}

void CMainPageDlg::initBeforeCheck()
{

	//��ʼ�����ݡ�
	UpdateData(TRUE);
	m_totalNum = 0;
	m_extractedLinkNum = 0;
	m_totalTestNum = 0;
	m_testItemNum = 0;
	m_pData->setUrl(CStrToStr(m_strOriUrl));
	m_pConfigDialog->OnBnClickedButton3();	//��ȫ������д�롣
}

HTREEITEM FindItem(const CString& name, CTreeCtrl& tree, HTREEITEM hRoot)
{
	// check whether the current item is the searched one  
	CString text = tree.GetItemText(hRoot);
	if (text.Compare(name) == 0)
		return hRoot;

	// get a handle to the first child item  
	HTREEITEM hSub = tree.GetChildItem(hRoot);
	// iterate as long a new item is found  
	while (hSub)
	{
		// check the children of the current item  
		HTREEITEM hFound = FindItem(name, tree, hSub);
		if (hFound)
			return hFound;

		// get the next sibling of the current item  
		hSub = tree.GetNextSiblingItem(hSub);
	}

	// return NULL if nothing was found  
	return NULL;
}



/*

//����ֻ����ͷ����
//pSQLiTest = new CSQLiTest(m_pData);
//pSQLiTest->saveConfiguration();
//pSQLiTest->loadConfiguration();
string headerStr;
//curl_global_init(CURL_GLOBAL_ALL);
//CURL *curl = curl_easy_init();
//if (curl) {
//	curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.8.191/sqli-labs-master/");
//	/* get us the resource without a body! */
//	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

//	curl_slist *chunk = NULL;
//	chunk = curl_slist_append(chunk, "Accept: text/html, application/xhtml+xml, */*");
//	chunk = curl_slist_append(chunk, "Accept-Language: zh-CN");
//	//chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/535.7 (KHTML, like Gecko)");
//	chunk = curl_slist_append(chunk, "User - Agent: Mozilla / 5.0 (Windows NT 10.0; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 56.0.2924.87 Safari / 537.36");
//	chunk = curl_slist_append(chunk, "Connection: keep-alive");
//	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
//	curl_easy_setopt(curl, CURLOPT_HEADEROPT, 1);
//	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &header_callback2);
//	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerStr);
//	curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
//	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteFunction2);


//	
//	/* Perform the request */
//	curl_easy_perform(curl);
//}
/*
// CHttpClient *pHttp = new CHttpClient();
// pHttp->setHeaderOpt("", headerStr);

pHttp->send(HttpMethod::get, "", "http://192.168.8.191/sqli-labs-master/Less-11/?uname='&passwd=&submit=Submit", "", TRUE);


*/




