#pragma once
#include "Item.h"
#include "CHttpClient.h"
class Test
{
public:
	Test();
	~Test();
	virtual bool test(CHttpClient *pHttpClient, Item *pItem) = 0;

};

