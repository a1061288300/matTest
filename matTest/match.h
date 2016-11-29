#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <io.h>
#include <stdlib.h>
#include <map>
#include <math.h>

#ifndef _MATCH_H_
#define _MATCH_H_
#endif

using namespace std;
using namespace cv;


map<char, double> matchImform;
map<char, double> WtoHArr;
const int total = 2500;
char* modePreName = "E:\\graduationDesign\\font\\ArialNarrow\\*.*";
char* matchPreName = "E:\\graduationDesign\\image\\275\\";

void getModeNorNameArr(char* modePreName, string modeNorNameArr[], string modeImformNameArr[])//获取XXnor.txt 与 XXimform.txt
{
	_finddata_t modeDir;
	string preName = (string)modePreName;
	preName = preName.substr(0, preName.length() - 3);
	long lfDir;
	int nortmp = 0, imformtmp = 0;
	if ((lfDir = _findfirst(modePreName, &modeDir)) == -1l)
	{
		cout << "找不到模板文件" << endl;
	}
	else
	{
		do{
			string modeName = (string)modeDir.name;
			
			if (modeName.length() >= 7 && (modeName.substr(modeName.length() - 7, 3) == "nor"))
			{
				modeNorNameArr[nortmp] = preName + modeName;
				nortmp++;
			}
			else if (modeName.length() >= 10 && (modeName.substr(modeName.length() - 10, 6) == "imform"))
			{
				modeImformNameArr[imformtmp] = preName + modeName;
				imformtmp++;
			}

		} while (_findnext(lfDir, &modeDir) == 0);
	}
	_findclose(lfDir);

}

void getMatchNameArr(char* matchPreName, string matchNameArr[])
{
	for (int i = 0; i < wordsNum; i++)
	{
		string matchTmpName = matchPreName;
		int tt = i;
		string ss;
		if (tt == 0)
			matchTmpName += '0';
		while (tt != 0)
		{
			int tmp = tt % 10;
			ss += (tmp + '0');
			tt /= 10;
		}

		reverse(ss.begin(), ss.end());
		matchTmpName += ss;
		matchTmpName += ".txt";
		matchNameArr[i] = matchTmpName;
	}
}

int getNorNumber(string ss)//获取以XXnor.txt文件名nor前面的数字
{
	string tmps = ss.substr(0, ss.length() - 7);
	int len = tmps.length() - 1;
	while (tmps[len] - '0' >= 0 && tmps[len] - '0' <= 9)
	{
		len--;
	};
	int num = 0;
	len++;
	while (tmps[len] - '0' >= 0 && tmps[len] - '0' <= 9)
	{
		num *= 10;
		num += (tmps[len] - '0');
		len++;
	}
	
	return num;
}

int getImformNumber(string ss)//获取以XXimform.txt文件名imform前面的数字
{
	string tmps = ss.substr(0, ss.length() - 10);
	int len = tmps.length() - 1;
	while (tmps[len] - '0' >= 0 && tmps[len] - '0' <= 9)
	{
		len--;
	}
	int num = 0;
	len++;
	while (tmps[len] - '0' >= 0 && tmps[len] - '0' <= 9)
	{
		num *= 10;
		num += (tmps[len] - '0');
		len++;
	}
	return num;
}

void getWtoHArr(wordImformation word, string matchName, string modeName)//得到宽高比  匹配的结果  bematch.whscale / mode.whscale
{

	double whscale = word.whScale;

	ifstream modeIn(modeName);
	char tmp[10];
	if (!modeIn.is_open())
	{
		cout << "打开宽高比文件失败" << endl;
		exit(1);
	}
	modeIn.getline(tmp, 10);
	double modeWHscale = atof(tmp);
	//cout << "modeWHscale: " << modeWHscale << endl;
	//cout << "whscale: " << whscale << endl; 
	int k = getImformNumber(modeName);
	if (modeWHscale == 0)
		WtoHArr[char(k)] = 0;
	else
	{
		WtoHArr[char(k)] = whscale / modeWHscale;
		//WtoHArr[char(k)] = 0;
		//cout << whscale << "\t" << modeWHscale << "\t";
		//cout << WtoHArr[char(k)] << endl;
	}

}


void getMatchArr(string matchName, string modeName)//得到像素匹配相似度
{

	char bematch[100];
	char mode[100];
	int len = strlen(bematch);

	ifstream matchIn(matchName);
	ifstream modeIn(modeName);

	if (!matchIn.is_open())
	{
		cout << "打开待匹配文件失败" << endl;
		exit(1);
	}
	if (!modeIn.is_open())
	{
		cout << "打开模板文件失败" << endl;
		exit(1);
	}


	int count = 0;

	for (int i = 0; i < 50; i++)
	{
		matchIn.getline(bematch, 100);
		modeIn.getline(mode, 100);
		int len = strlen(bematch);
		for (int j = 0; j < len; j++)
		{
			if (bematch[j] == mode[j])
			{
				count++;
			}
		}
	}
	//cout << "count: " << count << endl;
	//cout << "total: " << total << endl;
	double res = (double)count / (double)total;
	//cout << "res: " << res << endl;
	int k = getNorNumber(modeName);
	matchImform[char(k)] = res;
	matchIn.close();
	modeIn.close();
	
}

int modehorizon[50] = { 0 };
int modevertical[50] = { 0 };
int matchhorizon[50] = { 0 };
int matchvertical[50] = { 0 };
map<char, double> matchHVImform;
void getMatchHorVerArr(string matchName)
{
	for (int i = 0; i < 50; i++)
	{
		matchhorizon[i] = 0;
		matchvertical[i] = 0;
	}

	ifstream matchIn(matchName);
	if (!matchIn.is_open())
	{
		cout << "打开模板文件失败" << endl;
	}

	char matchline[50];
	for (int j = 0; j < 50; j++)
	{
		matchIn.getline(matchline, 51);
		int len = strlen(matchline);
		int count1 = 0;
		for (int k = 0; k < len; k++)
		{
			matchvertical[k] += matchline[k] - '0';
			count1 += matchline[k] - '0';
		}
		matchhorizon[j] = count1;
	}
	matchIn.close();
}

void getModeHorVerArr(string modeName)//获取模板中每行每列的像素信息 的 匹配程度
{
		for (int j = 0; j < 50; j++)
		{
			modehorizon[j] = 0;
			modevertical[j] = 0;
		}

		ifstream modeIn(modeName);
		if (!modeIn.is_open())
		{
			cout << "打开模板文件失败" << endl;
		}

		char modeline[50];
		for (int j = 0; j < 50; j++)
		{
			modeIn.getline(modeline, 51);
			int len = strlen(modeline);
			int count = 0;
			for (int k = 0; k < len; k++)
			{
				modevertical[k] += modeline[k] - '0';
				count += modeline[k] - '0';
			}
			modehorizon[j] = count;

		}

		modeIn.close();

}

double getHVArr(int modehorizon[], int modevertical[], int matchhorizon[], int matchvertical[])
{
	double sumV = 0, sumH = 0;
	for (int i = 0; i < 50; i++)
	{
		sumH += (modehorizon[i] - matchhorizon[i]) * (modehorizon[i] - matchhorizon[i]);
		sumV += (modevertical[i] - matchvertical[i]) * (modevertical[i] - matchvertical[i]);
	}
	double res = 0.0;
	res = 1 - (sumV / 10000 + sumH / 10000) / 2;
	//cout << "res: " << res << endl;
	return res;
}