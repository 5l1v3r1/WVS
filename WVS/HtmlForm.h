#pragma once
#include<string>
#include<vector>
#include "Tool.h"
#include "Field.h"

using namespace std;
class Field;

class HtmlForm
{
public:
	HtmlForm(string s);
	HtmlForm(){};
	string toString();
	HtmlForm operator=(HtmlForm&a);

	HttpMethod getMethod();
	void setMethod(HttpMethod val);
	std::string getAction();
	void setAction(std::string val);
	std::vector<Field> getFields() const { return m_fields; }
	void setFields(std::vector<Field> val) { m_fields = val; }

	static const  string INPUT_REGEX;
	static const  string TEXTAREA_REGEX;
	static const  string FORM_REGEX;
private:
	string m_action;
	HttpMethod m_method;
	vector<Field> m_fields;
};