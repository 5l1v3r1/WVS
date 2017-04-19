#pragma once
#include <string>
#include <vector>
#include "Tool.h"
#include "Cookie.h"
class Item
{
public:
	Item();
	~Item();
	
	Item(const Item&a);
	Item(HttpMethod method, string url);
	Item(HttpMethod method, string url, vector<Field> args);
	

	Item operator=(Item&a);
	bool equal(Item&a);
	bool hasExistArgByName(string name);

	HttpMethod getMethod() const { return m_method; }
	void setMethod(HttpMethod val) { m_method = val; }
	std::string getUrl()const  { return m_url; }
	void setUrl(std::string val) { m_url = val; }
	std::vector<Field> getArgs() const { return m_args; }
	void setArgs(std::vector<Field> val) { m_args = val; }
	int getLayer() const { return m_layer; }
	void setLayer(int val) { m_layer = val; }

private:
	HttpMethod m_method;
	string m_url;
	vector<Field> m_args;
	int m_layer;
};