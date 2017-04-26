#pragma once
#include<string>
#include<vector>
#include "Field.h"
#include "Tool.h"
using namespace std;

class Cookie
{
public:
	Cookie(){};
	Cookie(string headerStr);
	Cookie(const Cookie &a);
	string toString();
	bool operator==(Cookie&a);
	Cookie operator=(Cookie&a);

	static const string COOKIE_REGEX;
	static const string COOKIE_FIELD_REGEX;
	int hasExisted(string name);
private:
	vector<Field> m_cookie;

	
};
