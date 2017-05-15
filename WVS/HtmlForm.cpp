#include "stdafx.h"
#include"HtmlForm.h"
HtmlForm::HtmlForm(string s)
{
	vector<string> inputVec;
	findByRegex(s, INPUT_REGEX, inputVec, false);
	string value;
	Field *temp;
	for (unsigned int i = 0; i < inputVec.size(); i++)
	{
		temp = new Field();
		if (findByName(inputVec[i], string("type"), value, false))
		{
			temp->m_type = value;
		}
		if (findByName(inputVec[i], string("name"), value, false))		//problem 2:这里有意外情况：	<input id="userName" name="userName" placeholder="用户名" type="text" style="height:30px;"/>     Name" name="
		{
			temp->m_name = value;
		}
		if (findByName(inputVec[i], string("value"), value, false))
		{
			temp->m_value = value;
			
		}
		if (temp->getType() == "text" && temp->getValue() == "")
		{
			temp->m_defaultValue = temp->DEFAULT_NAME_VALUE;
		}
		else if (temp->getType() == "password" && temp->getValue() == "")
		{
			temp->m_defaultValue = temp->DEFAULT_PASSWORD; temp->DEFAULT_PASSWORD;
		}
		else if (_stricmp(temp->getType().c_str(), string("Submit").c_str()) == 0)
		{
			//temp->setSecurityFlag(1);
		}
		this->m_fields.push_back(*temp);
	}
	vector<string> vecTextArea;
	findByRegex(s, TEXTAREA_REGEX, vecTextArea, false);
	for (unsigned int i = 0; i < vecTextArea.size(); i++)
	{
		temp = new Field();
		if (findByName(vecTextArea[i], string("type"), value, false))
		{
			temp->m_type = value;
		}
		if (findByName(vecTextArea[i], string("name"), value, false))		//problem 2:这里有意外情况：	<input id="userName" name="userName" placeholder="用户名" type="text" style="height:30px;"/>     Name" name="
		{
			if (value.find("mtxMessage") != -1)
			{
				int x = 1;

			}
			temp->m_name = value;
		}
		if (vecTextArea.size()>i)
		{
			temp->m_value = vecTextArea[i+1];
			i++;
		}
		this->m_fields.push_back(*temp);
	}


	if (findByName(s, string("action"), value, false))
	{
		m_action = value;
	}
	if (findByName(s, string("method"), value, false))
	{
		transform(value.begin(), value.end(), value.begin(), tolower);	//将字符串全部变为小写。
		if ("post" == value)
			this->setMethod(HttpMethod::post);
		else if ("get" == value)
			this->setMethod(HttpMethod::get);
		else
			this->setMethod(HttpMethod::other);	//暂且无法处理这种情况。
	}
	

}

std::string HtmlForm::toString()
{
	string args;
	for (unsigned int i = 0; i < this->m_fields.size(); i++)
	{
		if (m_fields[i].getName() != "")
		{
			args += m_fields[i].m_name + "=" + m_fields[i].m_value + m_fields[i].m_defaultValue;
			//	cout << m_fields[i].m_name << "***" << m_fields[i].m_value << "***" << m_fields[i].m_defaultValue << "***" << endl;
			if (i < m_fields.size() - 1)
			{
				args += "&";
			}
		}
	}
	return args;
}

HtmlForm HtmlForm::operator=(HtmlForm&a)
{
	m_action = a.m_action;
	m_method = a.m_method;
	m_fields = a.m_fields;

	return *this;
}

const std::string HtmlForm::INPUT_REGEX = "<\\s*input[^>]*?>";
const std::string HtmlForm::TEXTAREA_REGEX = "<\\s*textarea[^>]*?>([^<]*)<\\s*/\\s*textarea\\s*>";
const std::string HtmlForm::FORM_REGEX = "<\\s*form[^]*?/\\s*form\\s*>";

HttpMethod HtmlForm::getMethod()
{
	return m_method;
}

void HtmlForm::setMethod(HttpMethod val)
{
	m_method = val;
}


std::string HtmlForm::getAction()
{
	return m_action;
}

void HtmlForm::setAction(std::string val)
{
	m_action = val;
}
