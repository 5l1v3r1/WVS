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
// Function:  展示向量里面的内容
//************************************
void showVecStr(vector<string> vec);

//************************************
// Method:    vecStrToString
// FullName:  vecStrToString
// Access:    public 
// Returns:   std::string
// Qualifier:
// Parameter: vector<string> vec
// Function:  将容器里面的字符串串接并返回
//************************************
string vecStrToString(vector<string> vec);

//************************************
// Method:    findName
// FullName:  findName
// Access:    public 
// Returns:   bool  是否找到了相应的bool值。
// Qualifier:
// Parameter: string s
// Parameter: string & name
// Parameter: string & value
// Parameter: bool isCaseSensitive
// Function:  在给定的s（即一个html标签<>中）,根据name来寻找其对应的value.
//************************************
bool findByName(string s, string name, string& value, bool isCaseSensitive);


//************************************
// Method:    findByRegex
// FullName:  findByRegex
// Access:    public 
// Returns:   int 匹配的字符串数量。
// Qualifier:
// Parameter: string s
// Parameter: string sRegex
// Parameter: vector<string> & sVex
// Parameter: bool isCaseSensitive
// Function:  根据给定的正则模式sRegex，在s中寻找能够匹配的字符串存储在sVex中并返回。
//************************************
int findByRegex(string s, string sRegex, vector<string>&sVex, bool isCaseSensitive);

//const string linkRegex = "<\\s*a[^>]>";	//辅以findByName  name = href
//const string baseRegex = "<\\s*base[^>]"; // name = href

void WriteLog(string log);

void WriteFile(string FileName, string content);


//************************************
// Method:    formatLink
// FullName:  formatLink
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string baseUrl ：从base标签或者该网页的网址中获得。
// Parameter: string & link	 ：从网页中提取出来的链接，需要被格式化，格式化之后为完整的链接不带参数。
// Parameter: string & args   ：从链接中截取出来的参数字符串，需要进一步被提取为field字段。
// Function: 
//************************************
void formatLink(string baseUrl, string &link, string &args);

//************************************
// Returns:   std::vector<Field>*
// Parameter: string argStr :  从get链接中提取出来的参数字符串。
// Function:   从get链接中提取出来的参数字符串使用field储存。
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