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
	string getArgsStr(int pos = -1, string inject = "", bool needDefaultValue = true, bool urlEncoding = true);

	Item operator=(Item&a);
	bool equal(Item&a);
	bool hasExistArgByName(string name);
	void hash();

	void setResultId(int argIndex, int resultId);

	HttpMethod getMethod() const { return m_method; }
	void setMethod(HttpMethod val) { m_method = val; }
	std::string getUrl()const  { return m_url; }
	void setUrl(std::string val) { m_url = val; }
	std::vector<Field> getArgs() const { return m_args; }
	void setArgs(std::vector<Field> val) { m_args = val; }
	unsigned getLayer() const { return m_layer; }
	void setLayer(unsigned val) { m_layer = val; }
	unsigned getId() const { return id; }
	void setId(unsigned val) { id = val; }
	unsigned getOriId() const { return oriId; }
	void setOriId(unsigned val) { oriId = val; }
	int getHash(){ return m_hash; }
	void setHash(int h){ m_hash = h; }
private:
	HttpMethod m_method;
	string m_url;
	vector<Field> m_args;
	unsigned m_layer;
	unsigned id;
	unsigned oriId;
	int m_hash = 0;	//保留url+args的hash值作为比较格式为 method:url?name&...;
	
};