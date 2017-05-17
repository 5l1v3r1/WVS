#include "stdafx.h"
#include "Item.h"


Item::Item()
{
	reset();
}


//************************************
// Returns:   std::string
// Parameter: int pos :  -1 不插入
// Parameter: string inject :
// Parameter: bool needDefaultValue :
// Parameter: bool urlEncoding : 是否将参数使用url编码；不编码，便于显示；编码，便于发送请求
// Function:  
//************************************
std::string Item::getArgsStr(int pos /*= -1*/, string inject /*= ""*/, bool needDefaultValue /*= true*/, bool urlEncoding /*= true*/)
{
	string args;
	string name;
	string value;
	for (unsigned i = 0; i < m_args.size(); i++)
	{
		if (i == pos)
		{
			name = m_args[i].getName();
			if (needDefaultValue)
			{
				value = m_args[i].getValue() + inject;
			}
			else{
				value = inject;
			}
			
		}
		else
		{
			name = m_args[i].getName();
			value = m_args[i].getValue();
		}


		if (urlEncoding)
		{
			name = escapeURL(name);
			value = escapeURL(value);
		}

		args = args +name + "=" + value;
		if (i < m_args.size() - 1)
		{
			args += "&";
		}
	}
	return args;
}




Item::~Item()
{
}

Item::Item(HttpMethod method, string url)
{
	m_method = method;
	m_url = url;
}

Item::Item(HttpMethod method, string url, vector<Field> args)
{
	m_method = method;
	m_url = url;
	m_args = args;
}


Item::Item(const Item&a)
{
	m_url = a.getUrl();
	m_method = a.getMethod();
	m_args = a.getArgs();
	m_layer = a.getLayer();
	m_hash = a.m_hash;

}

Item Item::operator=(Item& a)
{
	m_method = a.getMethod();
	m_args = a.getArgs();
	m_url = a.getUrl();
	m_layer = a.getLayer();
	m_hash = a.m_hash;
	return *this;
}


//************************************
// Returns:   bool
// Parameter: string name :
// Function:  这个Item里面的参数中有没有name为传入的。
//************************************
bool Item::hasExistArgByName(string name)
{
	for (vector<Field>::iterator it = m_args.begin(); it != m_args.end(); it++)
	{
		if (it->getName() == name)
			return true;
	}
	return false;
}

//************************************
// Returns:   bool
// Parameter: Item a :
// Function:  //当两个Item的网址相同，参数个数和参数名相同时，认为两个Item相同。//意外情况：某个Item的具有两个相同名称的参数   不过，当具有相同名称的参数时，服务器也无法区分。
//				新版本： 问题： 1. 未解决hash碰撞问题。 2. 当参数相同但是位置不同时，会被认为为2个不同的url。实际上是相同的。
						//保证，item被插入到队列前，已经调用过hash函数。
//************************************
bool Item::equal(Item& a)
{
	if (m_hash == 0)
	{
		hash();
	}
	return a.m_hash == m_hash;
	/*if (m_url == a.getUrl() && m_args.size() == a.getArgs().size())
	{
		for (unsigned int i = 0; i < m_args.size(); i++)
		{
			if (!a.hasExistArgByName(m_args[i].getName()))
				return false;
		}
		return true;
	}
	return false;*/
}

void Item::setResultId(int argIndex, int resultId)
{
	m_args[argIndex].setResultId(resultId);
}

void Item::hash()
{
	string arg="";
	for (unsigned int i = 0; i < m_args.size(); i++)
	{
		arg += m_args[i].getName();
		if (i != m_args.size() - 1)
			arg += "&";
	}
	m_hash = BKDRHash((to_string(m_method) + m_url + "?" + arg).c_str());
}

void Item::reset()
{
	m_method = HttpMethod::other;
	m_url = "";
	m_args.clear();
	m_layer = -1;
	id = -1;
	oriId = -1;
	m_hash = 0;
}
