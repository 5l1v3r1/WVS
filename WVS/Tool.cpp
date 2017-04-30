#include "stdafx.h"
#include "Tool.h"

static int s_i = 0;
void Print(string k)
{
	cout << s_i++ << "$$$" << k << "$$$" << endl;
}
void showVecStr(vector<string> vec)
{
	for_each(vec.begin(), vec.end(), Print);
}
string vecStrToString(vector<string> vec)
{
	string str = "";
	for (unsigned int i = 0; i < vec.size(); i++)
	{
		str += vec[i];
	}
	return str;
}

//http://stackoverflow.com/questions/3152241/case-insensitive-stdstring-find 
template<typename charT>
struct my_equal {
	my_equal(const std::locale& loc) : loc_(loc) {}
	bool operator()(charT ch1, charT ch2) {
		return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
	}
private:
	const std::locale& loc_;
};

// find substring (case insensitive)
template<typename T>
int ci_find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale())
{
	typename T::const_iterator it = std::search(str1.begin(), str1.end(),
												str2.begin(), str2.end(), my_equal<typename T::value_type>(loc));
	if (it != str1.end()) return it - str1.begin();
	else return -1; // not found
}


bool findByName(string s, string name, string& value, bool isCaseSensitive)
{
	int posOfName = -1;
	int posOfQuote = -1;
	int posOfReverseQuote = -1;
	char quote = '>';
	bool flag = false;	//�Ƿ��п���"="��
	int posOfEqual = -1;
	unsigned int i;
	while (!flag)
	{
		if (isCaseSensitive)
		{
			posOfName = s.find(name, 0);
		}
		else
		{
			posOfName = ci_find_substr(s, name);
		}
		if (posOfName != -1)
		{
			for (i = (posOfName + name.size()); i < s.size(); i++)
			{
				if (s[i] == '=')
				{
					flag = true;
					posOfEqual = i;
					break;
				}
				else if (s[i] == ' ')
				{
					continue;
				}
				else
				{
					//֮ǰ���Ǹ�name�Ǽٵģ���Ҫ����ƥ�䡣
					s = s.substr(posOfName + name.size(), s.size());
					break;
				}
			}
		}
		else{
			break;	//û���ҵ�name
		}
	}
	if (posOfName != -1)
	{
		unsigned int i;
		for (i = posOfEqual + 1; i < s.size(); i++)
		{
			if (s[i] == '"' || s[i] == '\'')
			{
				quote = s[i];
				posOfQuote = i;
				i++;
				break;
			}

		}
		for (; i < s.size(); i++)
		{
			if (s[i] == quote)
			{
				posOfReverseQuote = i;
				break;
			}
		}
		if (posOfQuote < posOfReverseQuote)
		{
			value = s.substr(posOfQuote + 1, posOfReverseQuote - posOfQuote - 1);
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}


int findByRegex(string s, string sRegex, vector<string>&sVex, bool isCaseSensitive)
{
	if (s.size() > 0)
	{
		string temp = s;
		regex e;
		if (isCaseSensitive)
		{
			e = regex(sRegex, std::regex_constants::icase);
		}
		else
		{
			e = regex(sRegex);
		}
		smatch m;
		smatch::iterator k;
		while (regex_search(temp, m, e))
		{
			for (k = m.begin(); k != m.end(); k++)
			{
				sVex.push_back((*k).str());
			}
			temp = m.suffix().str();
		}
	}
	else
	{
		return 0;
	}

	return sVex.size();
}


void WriteLog(string log)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	ofstream logOut("MyLog.txt");
	cout << st.wHour << ":" << st.wMinute << "." << st.wSecond << "." << st.wMilliseconds << endl;
	logOut << log << endl;
	logOut.close();
	OutputDebugStringA(log.c_str());
}

void WriteFile(string FileName, string content)
{
	ofstream logOut(FileName.c_str(), ios::out | ios::app);
	logOut << content << endl;
	logOut.close();
}

void formatLink(string baseUrl, string &link, string &args)
{
	//���Ƚ�ȡ��  ֮��Ĳ��֡�
	int posOfQuestionMark = link.find('?', 0);
	if (posOfQuestionMark >= 0)
	{
		args = link.substr(posOfQuestionMark + 1, link.size() - posOfQuestionMark - 1);
		link = link.substr(0, posOfQuestionMark);
	}
	//������/��ͷ�ĵ�ַ��problem 1  ��Ҫ��ǰ��ƴ����������
	if (link[0] == '/')
	{
		//���Ե�ַ  ֻ�ü����������ɡ�
		int pos = -1;		//  '/'��λ��
		//��ȡ����
		pos = baseUrl.find('/', 7);
		if (pos != -1)
		{
			baseUrl = baseUrl.substr(0, pos);  //ֻ��������  http://192.168.8.191      link��Ϊ /DVWA-master
		}
		else{
			WriteLog("������������⣡(baseUrl, link)" + baseUrl + "\t" + link);
		}
	}

	//����ȥ��../�Լ�./�����⣬�����޷��Ƚ�ȥ��    ��
	int posOfLastUnderline = -1;
	while (link.size() > 2)
	{
		if (link.substr(0, 2) == "./")
		{
			link = link.substr(2, link.size() - 2);
		}
		else if (link.size() > 3 && link.substr(0, 3) == string("../"))
		{
			posOfLastUnderline = baseUrl.rfind("/", baseUrl.size() - 2);
			if (posOfLastUnderline == -1)
			{
				//cout << "���������Ч!" << baseUrl.c_str() << "\t\t" << link.c_str() << endl;
				//WriteLog("���������Ч!" + baseUrl + "\t\t" + link);
			}
			else
			{
				baseUrl = baseUrl.substr(0, posOfLastUnderline + 1);
			}
			link = link.substr(3, link.size() - 3);
		}
		else{
			//cout << "��ǰ״̬!" << baseUrl.c_str() << "\t\t" << link.c_str() << endl;
			break;
		}
	}


	

	if ((ci_find_substr(link, string("http://")) == -1) && (ci_find_substr(link, string("https://")) == -1))
	{
		//û���ҵ�����Ϊ��Ե�ַ
		link = baseUrl + link;
	}

	//��#��β��ȥ��#��
	while (link.size() >= 1 && link[link.size() - 1] == '#'){
		link = link.substr(0, link.size() - 1);
	}
	//ȥ����β��.�ź�..��   http://192.168.8.191/DVWA-master/.   ����֮ǰ�Ѿ�ȥ����../��./���ԣ��˴����ֻ��λ��������һ�������

	////���ȥ����β�ĵ�� .    ��ʱ����..  ��
	if ((link.size() >= 2) && (link.substr(link.size() - 2, 2) == "/."))
	{
		link = link.substr(0, link.size() - 1);
	}
}


//�Ľ���formatlink������Ч��̫�͡�
//void formatLinkV2(string baseUrl, string &link, string &args)
//{
//	//���Ƚ�ȡ��  ֮��Ĳ��֡�
//	int posOfQuestionMark = link.find('?', 0);
//	if (posOfQuestionMark >= 0)
//	{
//		args = link.substr(posOfQuestionMark + 1, link.size() - posOfQuestionMark - 1);
//		link = link.substr(0, posOfQuestionMark);
//	}
//	//������/��ͷ�ĵ�ַ��problem 1  ��Ҫ��ǰ��ƴ����������
//	if (link[0] == '/')
//	{
//		//���Ե�ַ  ֻ�ü����������ɡ�
//		int pos = -1;		//  '/'��λ��
//		//��ȡ����
//		pos = baseUrl.find('/', 7);
//		if (pos != -1)
//		{
//			baseUrl = baseUrl.substr(0, pos);  //ֻ��������  http://192.168.8.191      link��Ϊ /DVWA-master
//		}
//		else{
//			WriteLog("������������⣡(baseUrl, link)" + baseUrl + "\t" + link);
//		}
//	}
//
//	//��#��β��ȥ��#��
//	while (link.size() >= 1 && link[link.size() - 1] == '#'){
//		link = link.substr(0, link.size() - 1);
//	}
//	//ȥ����β��.�ź�..��   http://192.168.8.191/DVWA-master/.   ����֮ǰ�Ѿ�ȥ����../��./���ԣ��˴����ֻ��λ��������һ�������
//
//
//
//	if ((ci_find_substr(link, string("http://")) == -1) && (ci_find_substr(link, string("https://")) == -1))
//	{
//		//û���ҵ�����Ϊ��Ե�ַ
//		link = baseUrl + link;
//	}
//
//	string domain;
//	int pos = link.find('/', 7);
//	if (pos != -1)
//	{
//		domain = link.substr(0, pos + 1);  //ֻ��������  http://192.168.8.191/      link��Ϊ /DVWA-master
//	}
//
//	//����ȥ��../�Լ�./�����⣬�����޷��Ƚ�ȥ��    �������  name../  
//	int posOfLastUnderline = -1;
//	int pos2;
//	int find = false;
//
//	while (link.size() > (int)(2 + pos + 1))
//	{
//		find = false;
//		pos2 = link.find("/./");
//		if (pos2 != -1)
//		{
//			link = link.substr(0, pos2) + link.substr(pos2 + 2);
//			find = true;
//		}
//
//		pos2 = link.find("/../");
//		if (pos2 != -1)
//		{
//			if (pos2 == pos)
//			{
//				link = link.substr(0, pos) + link.substr(pos2 + 3);
//			}
//			else
//			{
//				int pos3 = link.rfind("/", pos2 - 1);
//				assert(pos3 != -1);
//				link = link.substr(0, pos3) + link.substr(pos2 + 3);
//			}
//			find = true;
//		}
//		if (!find)
//		{
//			break;
//		}
//
//		//if (link.substr(0, 2) == "./")
//		//{
//		//	link = link.substr(2, link.size() - 2);
//		//}
//		//else if (link.size() > 3 && link.substr(0, 3) == string("../"))
//		//{
//		//	posOfLastUnderline = baseUrl.rfind("/", baseUrl.size() - 2);
//		//	if (posOfLastUnderline == -1)
//		//	{
//		//		//cout << "���������Ч!" << baseUrl.c_str() << "\t\t" << link.c_str() << endl;
//		//		//WriteLog("���������Ч!" + baseUrl + "\t\t" + link);
//		//	}
//		//	else
//		//	{
//		//		baseUrl = baseUrl.substr(0, posOfLastUnderline + 1);
//		//	}
//		//	link = link.substr(3, link.size() - 3);
//		//}
//		//else{
//		//	//cout << "��ǰ״̬!" << baseUrl.c_str() << "\t\t" << link.c_str() << endl;
//		//	break;
//		//}
//	}
//
//	//���ȥ����β�ĵ�� . ��..
//	if (link.substr(link.size() - 2, 2) == "/.")
//	{
//		link = link.substr(0, link.size() - 1);
//	}
//	else if (link.size() > (3 + pos) && link.substr(link.size() - 3, 3) == string("/.."))
//	{
//		pos2 = link.rfind("/", link.size() - 4);
//		if (pos2 < pos)
//		{
//			link = link.substr(0, link.size() - 2);
//		}
//		else
//		{
//			link = link.substr(0, pos2 + 1);
//		}
//	}
//}


vector<Field>* getAgrs(string argStr)
{

	vector<Field> *pArgs = NULL;
	Field *pTempFiled;
	regex e(argRegex);
	smatch m;
	smatch::iterator k;
	while (regex_search(argStr, m, e))
	{
		if (m.size() > 2)
		{
			if (pArgs == NULL){
				pArgs = new vector<Field>();
			}
			pTempFiled = new Field(m[1], m[2]);
			pArgs->push_back(*pTempFiled);
		}
		else
		{
			WriteLog("��������(rest url):" + argStr);
		}
		argStr = m.suffix().str();
		//cout << "ʣ�µ���:" << url << endl;
	}
	return pArgs;
}


char dec2hexChar(short int n) {
	if (0 <= n && n <= 9) {
		return char(short('0') + n);
	}
	else if (10 <= n && n <= 15) {
		return char(short('A') + n - 10);
	}
	else {
		return char(0);
	}
}

short int hexChar2dec(char c) {
	if ('0' <= c && c <= '9') {
		return short(c - '0');
	}
	else if ('a' <= c && c <= 'f') {
		return (short(c - 'a') + 10);
	}
	else if ('A' <= c && c <= 'F') {
		return (short(c - 'A') + 10);
	}
	else {
		return -1;
	}
}

string escapeURL(const string &URL)
{
	string result = "";
	for (unsigned int i = 0; i < URL.size(); i++) {
		char c = URL[i];
		if (
			('0' <= c && c <= '9') ||
			('a' <= c && c <= 'z') ||
			('A' <= c && c <= 'Z') ||
			c == '/' || c == '.'
			) {
			result += c;
		}
		else {
			int j = (short int)c;
			if (j < 0) {
				j += 256;
			}
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1 * 16;
			result += '%';
			result += dec2hexChar(i1);
			result += dec2hexChar(i0);
		}
	}
	return result;
}

string deescapeURL(const string &URL) {
	string result = "";
	for (unsigned int i = 0; i < URL.size(); i++) {
		char c = URL[i];
		if (c != '%') {
			result += c;
		}
		else {
			char c1 = URL[++i];
			char c0 = URL[++i];
			int num = 0;
			num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
			result += char(num);
		}
	}
	return result;
}

std::string CStrToStr(CString& cstr)
{
	const int bufInlen = 100;
	char buf[bufInlen];
	WideCharToMultiByte(CP_ACP, 0, cstr, -1, buf, bufInlen, NULL, NULL);
	return string(buf);
}

CString StrToCStr(string &str)
{
	const int bufInlen = 100;
	WCHAR wbuf[100];
	memset(wbuf, 0, 100 * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size() + 1, wbuf, bufInlen);
	return CString(wbuf);
}

void split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}


