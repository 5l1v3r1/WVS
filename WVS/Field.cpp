#include "stdafx.h"
#include "Field.h"

Field Field::operator=(Field &a)
{
	m_defaultValue = a.getDefaultValue();
	m_name = a.getName();
	m_type = a.getType();
	m_value = a.getValue();
	return *this;
}

bool Field::operator==(Field &a)
{
	if (m_name == a.getName() &&
		m_value == a.getValue() &&
		m_defaultValue == a.getDefaultValue())
		return true;
	return false;
}

std::string Field::toString()
{
	string arg = "";
	if (m_name != "")
	{
		arg = m_name + "=" + m_value + m_defaultValue;
		//	cout << m_fields[i].m_name << "***" << m_fields[i].m_value << "***" << m_fields[i].m_defaultValue << "***" << endl;
	}
	return arg;
}

const std::string Field::DEFAULT_NAME_VALUE = "admin";
const std::string Field::DEFAULT_PASSWORD = "password";