// MainPageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WVS.h"
#include "MainPageDlg.h"
#include "afxdialogex.h"


bool checkInLinks(Item &des, vector<Item>&crawlerLinksVec);

vector<Item> crawlerLinksItemVec;	//存储所有爬行的links
int crawlerNum = 0;					//已经爬行过的链接数量，作为crawlerLinksItemVec的指示器。
Cookie g_cookie;					//作为最新的cookie，所有线程共一个，每次访问后，都解析http头，一旦发现Cookie改变，则立即使用新的cookie。
bool cookieDirty = false;
list<string> linksList;
list<string> getList;
string htmlFile = "";							//存储所有的网页字符串。测试用。
string domain = "wangbaiyuan.cn/";  //"192.168.8.191/DVWA-master";	//域名常量字符串，作为格式化之后的去重使用。只测试本网站内的网址。

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
	WriteFile("网址树.txt", crawlerLinksItemVec[crawlerNum].getUrl() + string("\tMethod:") + (char)(crawlerLinksItemVec[crawlerNum].getMethod() + '0') + string("\tAEGUMENT:") + vecFieldToString(crawlerLinksItemVec[crawlerNum].getArgs()));
}

void extractLinks()
{
	CHttpClient httpClient = CHttpClient();
	string myResponse = "";
	string headerStr = "";

	httpClient.setHeaderOpt("", headerStr);
	CURLcode code;

	vector<string>linksVec;	//暂存 从一个网页中提取的多个links，
	vector<string>baseVec;	//暂存 从一个网页中提取的base
	vector<string>formStrVec;//暂存  form
	vector<HtmlForm>formVec; //暂存 
	Item *tempItem;

	while (crawlerLinksItemVec.size() > crawlerNum && crawlerNum <200)
	{
		showCrawler();
		tempItem = &crawlerLinksItemVec[crawlerNum++];
		//每次当cookie修改之后，之后所有的请求都使用新的cookie。一般情况下，cookie都是静态的，即一次会话只使用一个不变的cookie。
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
							  WriteFile("网址树_2.txt", "curle_ok");

							  break;
			}
			default: {
						 WriteFile("网址树_2.txt", std::to_string(code));
						 break;
			}
		}
		if (code == CURLE_COULDNT_CONNECT)		//无法链接服务器，则直接进行测试下一个链接
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

		//1. 获取baseUrl。 首先寻找base标签，如果没有则获取该网页地址去除最后一段用‘\’分隔开的字符串作为baseurl。
		findByRegex(myResponse, baseRegex, baseVec, false);
		string baseUrl;
		if (baseVec.size() > 1)
		{
			string FileName = "Base" + tempItem->getUrl();
			WriteLog("多于一个base标签:" + vecStrToString(baseVec) + "\n原文见文件:" + FileName + "\n");
			baseUrl = tempItem->getUrl();
		}
		else if (baseVec.size() == 1)
		{
			if (!findByName(baseVec[0], "href", baseUrl, false))
			{
				WriteLog("BaseUrl提取失败: baseTag = " + baseVec[0] + "提取内容:" + baseUrl.c_str() + "\n");
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
			WriteLog("Url 太短,里面没有/，只有协议（http://）:" + tempItem->getUrl() + "\n");
			exit(1);
		}
		else		{
			baseUrl = baseUrl.substr(0, posOfEnd + 1);
		}


		//2. 获取网页内所有的<a>标签，提取其中的href字段，格式化为Item,设置请求方式为get。去重比较，若不重复则加入待扫描队列。
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
				//	cout << "这个链接已经存在过！" << pTempNewItem->getUrl() << endl;
				delete pTempNewItem;
			}
		}

		//3. 获取所有的form表单。对每个表单中的input标签进行提取并格式化为Field。最后将每个表单存储为Item,设置请求方式为post。去重比较，若不重复则加入待扫描队列。
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
				//	cout << "这个链接已经存在过！" << pTempNewItem->getUrl() << endl;
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
// Returns:   bool   存在过：true;    未出现过false；
// Parameter: Item & des :
// Parameter: vector<Item> & crawlerLinksVec :
// Function:  查看是否已经存在过。
//************************************
bool checkInLinks(Item &des, vector<Item>&crawlerLinksVec)
{
	if (des.getUrl().find(domain) == -1)
	{
		cout << "不属于本网站的网址！" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().size() >= 5 && (des.getUrl().c_str()[4] == 's' || des.getUrl().c_str()[4] == 'S'))
	{
		cout << "该网址为https,暂不支持!" << des.getUrl() << endl;
		return true;
	}
	else if (des.getUrl().find("logout") != -1)
	{
		cout << "退出登陆网址，不要" << des.getUrl() << endl;
		return true;
	}
	for (int i = 0; i < crawlerLinksItemVec.size(); i++)
	{
		if (des.equal(crawlerLinksItemVec[i]))
		{
			cout << "这个链接已经存在过！" << des.getUrl() << endl;
			return true;
		}
	}
	cout << "新链接！" << des.getUrl() << endl;
	return false;
}


// CMainPageDlg 对话框

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

// CMainPageDlg 消息处理程序



void CMainPageDlg::OnEnChangeNetaddress1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	//m_oriUrl.DisplayErrorTip();
}


void CMainPageDlg::OnBnClickedBegin()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData(TRUE);
	
	m_totalNum = 100;
	m_crawledNum = "50(%50)";
	m_totalTestNum = 200;
	const int bufInlen = 100;
	//char buf[bufInlen] = L"中文asdf";
	//memset(buf, 0, bufInlen);
	//WCHAR *wch = (WCHAR*)m_strOriUrl.GetBuffer(m_strOriUrl.GetLength());
	// WCHRA to char  
	//WideCharToMultiByte(CP_ACP, 0, wch, -1, buf, bufInlen, NULL, NULL);
	_cprintf("%d,%d,%s中文\n", m_totalNum, m_totalTestNum, buf);
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
