#include "stdafx.h"
#include <iostream>
#include <cstring>
#include "nameFunc.h"
using namespace std;

char* yangPicName(char* tmp_name, char* add_string, int n)
{
	char* res = "a";
	int lenss = strlen(tmp_name);
	int lenadd = strlen(add_string);
	for (int i = 0; i < lenss; i++)
	{
		if (i < lenss - n)
			res[i] = tmp_name[i];
		if (i >= lenss - n && i < lenss - n + lenadd)
		{
			res[i] = add_string[i - lenss + n];
		}
	}
	return res;
}