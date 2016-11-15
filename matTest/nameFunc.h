#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
using namespace std;

#ifndef _NAMEFUNC_H_
#define _NAMEFUNC_H_
#endif

string getPicName(string tmp_name, string add_string, int n)
{
	tmp_name = tmp_name.substr(0, tmp_name.length() - n);
	tmp_name += add_string;
	return tmp_name;
}