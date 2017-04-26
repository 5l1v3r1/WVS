#include "stdafx.h"
#include "Item.h"


Item::Item()
{
}


std::string Item::getArgsStr(int pos, string inject)
{
	string args;
	for (unsigned i = 0; i < m_args.size(); i++)
	{
		args = args + m_args[i].getName() + "=" + m_args[i].getValue();
		if (i == pos)
		{
			args += inject;
		}
		if (i <= args.size() - 1)
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
}

Item Item::operator=(Item& a)
{
	m_method = a.getMethod();
	m_args = a.getArgs();
	m_url = a.getUrl();
	m_layer = a.getLayer();
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
//************************************
bool Item::equal(Item& a)
{

	if (m_url == a.getUrl() && m_args.size() == a.getArgs().size())
	{
		for (unsigned int i = 0; i < m_args.size(); i++)
		{
			if (!a.hasExistArgByName(m_args[i].getName()))
				return false;
		}
		return true;
	}
	return false;
}

