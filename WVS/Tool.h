#pragma  once
#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <queue>
#include <windows.h>
#include <fstream>
#include "Field.h"

using namespace std;

typedef enum HttpMethod{
	post = 1,
	get,
	other
}HttpMethod;



//************************************
// Method:    showVecStr
// FullName:  showVecStr
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: vector<string> vec
// Function:  չʾ�������������
//************************************
void showVecStr(vector<string> vec);

//************************************
// Method:    vecStrToString
// FullName:  vecStrToString
// Access:    public 
// Returns:   std::string
// Qualifier:
// Parameter: vector<string> vec
// Function:  ������������ַ������Ӳ�����
//************************************
string vecStrToString(vector<string> vec);

//************************************
// Method:    findName
// FullName:  findName
// Access:    public 
// Returns:   bool  �Ƿ��ҵ�����Ӧ��boolֵ��
// Qualifier:
// Parameter: string s
// Parameter: string & name
// Parameter: string & value
// Parameter: bool isCaseSensitive
// Function:  �ڸ�����s����һ��html��ǩ<>�У�,����name��Ѱ�����Ӧ��value.
//************************************
bool findByName(string s, string name, string& value, bool isCaseSensitive);


//************************************
// Method:    findByRegex
// FullName:  findByRegex
// Access:    public 
// Returns:   int ƥ����ַ���������
// Qualifier:
// Parameter: string s
// Parameter: string sRegex
// Parameter: vector<string> & sVex
// Parameter: bool isCaseSensitive
// Function:  ���ݸ���������ģʽsRegex����s��Ѱ���ܹ�ƥ����ַ����洢��sVex�в����ء�
//************************************
int findByRegex(string s, string sRegex, vector<string>&sVex, bool isCaseSensitive);

//const string linkRegex = "<\\s*a[^>]>";	//����findByName  name = href
//const string baseRegex = "<\\s*base[^>]"; // name = href

void WriteLog(string log);

void WriteFile(string FileName, string content);


//************************************
// Method:    formatLink
// FullName:  formatLink
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string baseUrl ����base��ǩ���߸���ҳ����ַ�л�á�
// Parameter: string & link	 ������ҳ����ȡ���������ӣ���Ҫ����ʽ������ʽ��֮��Ϊ���������Ӳ���������
// Parameter: string & args   ���������н�ȡ�����Ĳ����ַ�������Ҫ��һ������ȡΪfield�ֶΡ�
// Function: 
//************************************
void formatLink(string baseUrl, string &link, string &args);

//************************************
// Returns:   std::vector<Field>*
// Parameter: string argStr :  ��get��������ȡ�����Ĳ����ַ�����
// Function:   ��get��������ȡ�����Ĳ����ַ���ʹ��field���档
//************************************
vector<Field>* getAgrs(string argStr);


void extractUrl(string s, vector<string> urlVec);
void formatUrl(string baseUrl, string url);

bool findByRegex(string s, string regex, string &value, int flag);

const string baseRegex = "<\\s*base[^>]*>";
const string linksRegex = "<\\s*a[^>]*>";
const string argRegex = "([^=&]*)=([^=&]*)";

string deescapeURL(const string &URL);
string escapeURL(const string &URL);