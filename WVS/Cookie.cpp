#include "stdafx.h"
#include "Cookie.h"
Cookie::Cookie(string headerStr)
{
	string cookieStr;
	vector<string>cookieVec;
	vector<string>cookieFieldVec;
	Field *temp;
	int posOfEqule;
	if (findByRegex(headerStr, COOKIE_REGEX, cookieVec, false))
	{
		//showVecStr(cookieVec);
		for (int i = 0; i < cookieVec.size(); i++)
		{
			if (findByRegex(cookieVec[i], COOKIE_FIELD_REGEX, cookieFieldVec, false))
			{
				//cout << "cookieFieldVec:" <<i<< endl;
				//showVecStr(cookieFieldVec);
				for (int j = 0; j < cookieFieldVec.size(); j++)
				{
					temp = new Field();
					posOfEqule = cookieFieldVec[j].find("=");
					temp->setName(cookieFieldVec[j].substr(0, posOfEqule));
					temp->setValue(cookieFieldVec[j].substr(posOfEqule + 1, cookieFieldVec[j].size() - posOfEqule - 1));
					if (hasExisted(temp->getName()) || temp->getName() == "path" || temp->getName() == "httponly")
					{
						//cout << "delete field:" << temp->m_name << endl;
						delete temp;
						continue;	//即不发送名称为path或者httponly的cookie字段。名称相同的字段也只发送一个，值为第一次的值
					}
					m_cookie.push_back(*temp);
				}
				cookieFieldVec.clear();
			}
		}
	}
}

Cookie::Cookie(const Cookie&a)
{
	m_cookie = a.m_cookie;
}

std::string Cookie::toString()
{
	string args;
	for (int i = 0; i < this->m_cookie.size(); i++)
	{
		if (m_cookie[i].getName() != "")
		{
			args += m_cookie[i].getName() + "=" + m_cookie[i].getValue();
			//cout << m_cookie[i].m_name << "***" << m_cookie[i].m_value << "***"  << endl;
			if (i < this->m_cookie.size() - 1)
			{
				args += "; ";
			}
		}
	}
	return args;
}

//************************************
// Returns:   bool
// Parameter: Cookie & a :
// Function:  当两个cookie的参数个数且名称，值，默认值都相同时，认为两个cookie相同。
//************************************
bool Cookie::operator==(Cookie&a)
{
	if (m_cookie.size() == a.m_cookie.size())
	{
		int i, j;
		for (i = 0; i < m_cookie.size(); i++)
		{
			for (j = 0; j < a.m_cookie.size(); j++)
			{
				if (m_cookie[i] == a.m_cookie[j])
					break;
			}
			if (j == a.m_cookie.size())
				return false;
		}
		return true;
	}
	return false;
}

//************************************
// Returns:   bool
// Parameter: string name :
// Function:  当某个cookie字段名已经存在于cookie中，则认为已经有了。
//************************************
bool Cookie::hasExisted(string name)
{
	for (int i = 0; i < m_cookie.size(); i++)
	{
		if (m_cookie[i].getName() == name)
			return true;
	}
	return false;
}

const std::string Cookie::COOKIE_REGEX = "Set-Cookie.*";

const std::string Cookie::COOKIE_FIELD_REGEX = "[^; =]*=[^; =]*";	//没有=号的cookie字段直接舍弃。


