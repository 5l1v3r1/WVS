#include "stdafx.h"
#include "Item.h"


Item::Item()
{

}


std::string Item::getArgsStr(int pos /*= -1*/, string inject /*= ""*/, bool needDefaultValue /*= true*/)
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
				value += m_args[i].getValue() + inject;
			}
			else{
				value += inject;
			}
			
		}
		else
		{
			name = m_args[i].getName();
			value = m_args[i].getValue();
		}
		name = escapeURL(name);
		value = escapeURL(value);

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
// Function:  ���Item����Ĳ�������û��nameΪ����ġ�
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
// Function:  //������Item����ַ��ͬ�����������Ͳ�������ͬʱ����Ϊ����Item��ͬ��//���������ĳ��Item�ľ���������ͬ���ƵĲ���   ��������������ͬ���ƵĲ���ʱ��������Ҳ�޷����֡�
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
