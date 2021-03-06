#pragma once
#include<string>
using namespace std;

class Field
{
public:
	friend class HtmlForm;
	friend class Cookie;

	Field(){};
	Field(string name, string value) :m_name(name), m_value(value){};
	Field operator=(Field&a);
	bool operator==(Field&a);
	string toString();


	std::string getValue() const { return m_value; }
	void setValue(std::string val) { m_value = val; }
	std::string getName() const { return m_name; }
	void setName(std::string val) { m_name = val; }
	std::string getType() const { return m_type; }
	void setType(std::string val) { m_type = val; }
	std::string getDefaultValue() const { return m_defaultValue; }
	void setDefaultValue(std::string val) { m_defaultValue = val; }
	unsigned getSecurityFlag() const { return securityFlag; }
	void setSecurityFlag(unsigned val) { securityFlag  = securityFlag|val; }
	int getResultId() const { return m_resultId; }
	void setResultId(int val) { m_resultId = val; }

	

private:
	string m_name;
	string m_value;
	string m_defaultValue;
	string m_type;
	unsigned securityFlag = 0;	//作为肯定安全的标记，这个参数不用测试默认为0，需要测试
	int m_resultId = -1;		//漏洞具体情况ID， -1代表没有漏洞；
	
	static  const string DEFAULT_NAME_VALUE;
	static  const string DEFAULT_PASSWORD;
	
};