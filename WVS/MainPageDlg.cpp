// MainPageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WVS.h"
#include "MainPageDlg.h"
#include "afxdialogex.h"


bool checkInLinks(Item &des, vector<Item>&crawlerLinksVec);

vector<Item> crawlerLinksItemVec;	//�洢�������е�links
int crawlerNum = 0;					//�Ѿ����й���������������ΪcrawlerLinksItemVec��ָʾ����
Cookie g_cookie;					//��Ϊ���µ�cookie�������̹߳�һ����ÿ�η��ʺ󣬶�����httpͷ��һ������Cookie�ı䣬������ʹ���µ�cookie��
bool cookieDirty = false;
list<string> linksList;
list<string> getList;
string htmlFile = "";							//�洢���е���ҳ�ַ����������á�
string domain = "wangbaiyuan.cn/";  //"192.168.8.191/DVWA-master";	//���������ַ�������Ϊ��ʽ��֮���ȥ��ʹ�á�ֻ���Ա���վ�ڵ���ַ��

string vecFieldToString(vector<Field> fieldVec)
{
	string args;
	for (int i = 0; i < fieldVec.size(); i++)
	{
		args += fieldVec[i].toString();
		if (i != fieldVec.size() - 1)
		{
			args += "&";
		}
	}
	return args;
}
void showCrawler()
{
	cout << "-------------------------------------------------------------------" << endl;
	cout << "url:" << crawlerLinksItemVec[crawlerNum].getUrl().c_str() << endl;
	cout << "cookie:" << g_cookie.toString() << endl;
	cout << "method:" << crawlerLinksItemVec[crawlerNum].getMethod() << endl;
	cout << "args:" << vecFieldToString(crawlerLinksItemVec[crawlerNum].getArgs()) << endl;
	cout << crawlerNum << endl;
	cout << "-------------------------------------------------------------------" << endl << endl;
	WriteFile("��ַ��.txt", crawlerLinksItemVec[crawlerNum].getUrl() + string("\tMethod:") + (char)(crawlerLinksItemVec[crawlerNum].getMethod() + '0') + string("\tAEGUMENT:") + vecFieldToString(crawlerLinksItemVec[crawlerNum].getArgs()));
}

void extractLinks()
{
	CHttpClient httpClient = CHttpClient();
	string myResponse = "";
	string headerStr = "";

	httpClient.setHeaderOpt("", headerStr);
	CURLcode code;

	vector<string>linksVec;	//�ݴ� ��һ����ҳ����ȡ�Ķ��links��
	vector<string>baseVec;	//�ݴ� ��һ����ҳ����ȡ��base
	vector<string>formStrVec;//�ݴ�  form
	vector<HtmlForm>formVec; //�ݴ� 
	Item *tempItem;

	while (crawlerLinksItemVec.size() > crawlerNum && crawlerNum <200)
	{
		showCrawler();
		tempItem = &crawlerLinksItemVec[crawlerNum++];
		//ÿ�ε�cookie�޸�֮��֮�����е�����ʹ���µ�cookie��һ������£�cookie���Ǿ�̬�ģ���һ�λỰֻʹ��һ�������cookie��
		if (cookieDirty)
		{
			cookieDirty = false;
			code = httpClient.send(tempItem->getMethod(), g_cookie.toString(), tempItem->getUrl(), tempItem->getArgs(), myResponse);
		}
		else
		{
			code = httpClient.send(tempItem->getMethod(), "", tempItem->getUrl(), tempItem->getArgs(), myResponse);
		}
		switch (code)
		{
			/*
			CURLE_OK: printf("send ok!\n");
			CURLE_HTTP_POST_ERROR: printf("post error!\n");
			CURLE_COULDNT_CONNECT: printf("cannot connect to server\n");
			*/

			case CURLE_OK:{
							  WriteFile("��ַ��_2.txt", "curle_ok");

							  break;
			}
			default: {
						 WriteFile("��ַ��_2.txt", std::to_string(code));
						 break;
			}
		}
		if (code == CURLE_COULDNT_CONNECT)		//�޷����ӷ���������ֱ�ӽ��в�����һ������
		{
			continue;
		}

		htmlFile += myResponse;

		Cookie cookie(headerStr);
		if (!(cookie == g_cookie))
		{
			g_cookie = cookie;
			cookieDirty = true;
		}

		//1. ��ȡbaseUrl�� ����Ѱ��base��ǩ�����û�����ȡ����ҳ��ַȥ�����һ���á�\���ָ������ַ�����Ϊbaseurl��
		findByRegex(myResponse, baseRegex, baseVec, false);
		string baseUrl;
		if (baseVec.size() > 1)
		{
			string FileName = "Base" + tempItem->getUrl();
			WriteLog("����һ��base��ǩ:" + vecStrToString(baseVec) + "\nԭ�ļ��ļ�:" + FileName + "\n");
			baseUrl = tempItem->getUrl();
		}
		else if (baseVec.size() == 1)
		{
			if (!findByName(baseVec[0], "href", baseUrl, false))
			{
				WriteLog("BaseUrl��ȡʧ��: baseTag = " + baseVec[0] + "��ȡ����:" + baseUrl.c_str() + "\n");
				baseUrl = tempItem->getUrl();
			}
		}
		else
		{
			baseUrl = tempItem->getUrl();
		}
		cout << baseUrl << endl;
		int posOfEnd = baseUrl.rfind('/');
		if (posOfEnd < 6)
		{
			WriteLog("Url ̫��,����û��/��ֻ��Э�飨http://��:" + tempItem->getUrl() + "\n");
			exit(1);
		}
		else		{
			baseUrl = baseUrl.substr(0, posOfEnd + 1);
		}


		//2. ��ȡ��ҳ�����е�<a>��ǩ����ȡ���е�href�ֶΣ���ʽ��ΪItem,��������ʽΪget��ȥ�رȽϣ������ظ�������ɨ����С�
		findByRegex(myResponse, linksRegex, linksVec, false);
		string tempLink;
		Item *pTempNewItem;
		vector<Field>*pArgs;
		string argStr;
		for (int i = 0; i < linksVec.size(); i++)
		{
			pTempNewItem = new Item();
			argStr = "";
			findByName(linksVec[i], "href", tempLink, false);
			formatLink(baseUrl, tempLink, argStr);
			pTempNewItem->setUrl(tempLink);
			if ((pArgs = getAgrs(argStr)) != NULL)
			{
				pTempNewItem->setArgs(*pArgs);
				delete pArgs;
			}
			pTempNewItem->setMethod(HttpMethod::get);

			if (!checkInLinks(*pTempNewItem, crawlerLinksItemVec))
			{
				crawlerLinksItemVec.push_back(*pTempNewItem);
			}
			else
			{
				//	cout << "��������Ѿ����ڹ���" << pTempNewItem->getUrl() << endl;
				delete pTempNewItem;
			}
		}

		//3. ��ȡ���е�form������ÿ�����е�input��ǩ������ȡ����ʽ��ΪField�����ÿ�����洢ΪItem,��������ʽΪpost��ȥ�رȽϣ������ظ�������ɨ����С�
		findByRegex(myResponse, HtmlForm::FORM_REGEX, formStrVec, false);
		HtmlForm* form;
		for (int i = 0; i < formStrVec.size(); i++)
		{
			form = new HtmlForm(formStrVec[i]);
			cout << form->toString().c_str() << endl;
			formVec.push_back(*form);
		}
		for (int i = 0; i < formVec.size(); i++)
		{
			tempLink = formVec[i].getAction();
			pTempNewItem = new Item();
			formatLink(baseUrl, tempLink, argStr);
			pTempNewItem->setUrl(tempLink);
			pTempNewItem->setMethod(HttpMethod::post);
			pTempNewItem->setArgs(formVec[i].getFields());
			if (!checkInLinks(*pTempNewItem, crawlerLinksItemVec))
			{
				crawlerLinksItemVec.push_back(*pTempNewItem);
			}
			else
			{
				//	cout << "��������Ѿ����ڹ���" << pTempNewItem->getUrl() << endl;
				delete pTempNewItem;
			}
		}

		headerStr = "";
		myResponse = "";
		linksVec.clear();
		formVec.clear();
		baseVec.clear();
		formStrVec.clear();
	}
}



//************************************
// Returns:   bool   ���ڹ���true;    δ���ֹ�false��
// Parameter: Item & des :
// Parameter: vector<Item> & crawlerLinksVec :
// Function:  �鿴�Ƿ��Ѿ����ڹ���
//************************************
bool checkInLinks(Item &des, vector<Item>&crawlerLinksVec)
{
	if (des.getUrl().find(domain) == -1)
	{
		cout << "�����ڱ���վ����ַ��" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().size() >= 5 && (des.getUrl().c_str()[4] == 's' || des.getUrl().c_str()[4] == 'S'))
	{
		cout << "����ַΪhttps,�ݲ�֧��!" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().find("logout") != -1)
	{
		cout << "�˳���½��ַ����Ҫ" << des.getUrl() << endl;
		return true;
	}
	for (int i = 0; i < crawlerLinksItemVec.size(); i++)
	{
		if (des.equal(crawlerLinksItemVec[i]))
		{
			cout << "��������Ѿ����ڹ���" << des.getUrl() << endl;
			return true;
		}
	}
	cout << "�����ӣ�" << des.getUrl() << endl;
	return false;
}


// CMainPageDlg �Ի���

IMPLEMENT_DYNAMIC(CMainPageDlg, CDialogEx)

CMainPageDlg::CMainPageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainPageDlg::IDD, pParent)
	, m_totalNum(0)
	, m_crawledNum(_T(""))
	, m_totalTestNum(0)
	, m_strOriUrl(_T(""))
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
	DDX_Text(pDX, IDC_CrawledNum_EDIT, m_crawledNum);
	DDX_Text(pDX, IDC_TotalTestNum_EDIT, m_totalTestNum);
	DDX_Text(pDX, IDC_EDIT1, m_strOriUrl);
}


BEGIN_MESSAGE_MAP(CMainPageDlg, CDialogEx)
	
	ON_EN_CHANGE(IDC_NETADDRESS1, &CMainPageDlg::OnEnChangeNetaddress1)
	ON_BN_CLICKED(ID_BEGIN, &CMainPageDlg::OnBnClickedBegin)
END_MESSAGE_MAP()

// CMainPageDlg ��Ϣ�������



void CMainPageDlg::OnEnChangeNetaddress1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//m_oriUrl.DisplayErrorTip();
}


void CMainPageDlg::OnBnClickedBegin()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE);
	
	m_totalNum = 100;
	m_crawledNum = "50(%50)";
	m_totalTestNum = 200;
	const int bufInlen = 100;
	//char buf[bufInlen] = L"����asdf";
	//memset(buf, 0, bufInlen);
	//WCHAR *wch = (WCHAR*)m_strOriUrl.GetBuffer(m_strOriUrl.GetLength());
	// WCHRA to char  
	//WideCharToMultiByte(CP_ACP, 0, wch, -1, buf, bufInlen, NULL, NULL);
	_cprintf("%d,%d,%s����\n", m_totalNum, m_totalTestNum, buf);
	UpdateData(FALSE);

	/*string baseUrl = "http://wangbaiyuan.cn/";
	Item item = Item(HttpMethod::get, baseUrl);

	crawlerLinksItemVec.push_back(item);
	extractLinks();
	cout << endl << "asdasdfasdfasdfasdfaasdfasdff" << endl;
	ofstream out_2("d:\\tmp\\HDHDC.html");
	out_2 << htmlFile;
	out_2.close();*/



}
