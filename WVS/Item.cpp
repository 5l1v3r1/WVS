#include "stdafx.h"
#include "Item.h"


Item::Item()
{
	reset();
}


//************************************
// Returns:   std::string
// Parameter: int pos :  -1 ������
// Parameter: string inject :
// Parameter: bool needDefaultValue :
// Parameter: bool urlEncoding : �Ƿ񽫲���ʹ��url���룻�����룬������ʾ�����룬���ڷ�������
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
//				�°汾�� ���⣺ 1. δ���hash��ײ���⡣ 2. ��������ͬ����λ�ò�ͬʱ���ᱻ��ΪΪ2����ͬ��url��ʵ��������ͬ�ġ�
						//��֤��item�����뵽����ǰ���Ѿ����ù�hash������
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
