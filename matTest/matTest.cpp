// matTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <string.h>
#include <time.h>
#include <cstring>
#include <algorithm>
#include <map>
#include <vector>
#include <fstream>
#include <io.h>
#include "grayTest.h"
#include "denose.h"
#include "binaryTest.h"
#include "resizeTest.h"
#include "rotat.h"
#include "roiTest.h"
#include "pixelToTxt.h"
#include "cutTest.h"
#include "makeFont.h"
#include "match.h"
#include "beMatchingMeta.h"
//#include "mess.h"
using namespace std;
using namespace cv;

int cmp(pair<char, double>& x, pair<char, double>& y)
{
	return x.second > y.second;
}

int main()
{
	clock_t start, finish;
	start = clock();

	const string nameStr = "E:\\graduationDesign\\image\\275\\275.jpg";
	Mat imageRead = imread(nameStr);
	cout << nameStr << endl;
	imageRead = resizeImage(imageRead, nameStr);
	Mat imageRead2 = GrayIma(imageRead, nameStr);
	Mat imageRead3 = getBinaryImage(imageRead2, nameStr);
	//Mat imageRead4 = getMouseRoi(imageRead3, nameStr);
	Mat imageRead4 = getRoi(imageRead3, nameStr);
	Mat imageRead5 = getBestAngleImg(imageRead4, nameStr);
	Mat label;
	Mat imageRead6 = getDenose(imageRead5,  nameStr, label);
	//getPixelTxt(imageRead6);
	wordImformation words[1000];
	getWordImformation(imageRead6, words);
	

	for (int k = 0; k < wordsNum; k++)//将分割出来的字符以50 * 50的01矩阵存在txt中
	{
		string preName = "E:\\graduationDesign\\image\\275\\";
		string pretmp;

		//cout << k << ":" << endl;
		int tmp = k;
		if (tmp == 0)
		{
			pretmp = "0";
		}
		while (tmp > 0)
		{
			int tt = tmp % 10;
			pretmp += ('0' + tt);
			tmp /= 10;
		}
		reverse(pretmp.begin(), pretmp.end());
		string endName = ".txt";
		string name = preName + pretmp + endName;
		getMatchWordTxt(imageRead6, words[k].up_line, words[k].down_line, words[k].left_line, words[k].right_line, name);
	}



	char* modePreName = "E:\\graduationDesign\\font\\ArialNarrow\\*.*";
	char* matchPreName = "E:\\graduationDesign\\image\\275\\";
	
	string modeNorNameArr[500];
	string modeImformNameArr[500];
	string matchNameArr[500];
	getModeNorNameArr(modePreName, modeNorNameArr, modeImformNameArr);
	getMatchNameArr(matchPreName, matchNameArr);
	
	for (int i = 0; i < wordsNum; i++)
	{
		for (int j = 0; modeNorNameArr[j] != ""; j++)
		{
			getMatchArr(matchNameArr[i], modeNorNameArr[j]);
			getWtoHArr(words[i], matchNameArr[i], modeImformNameArr[j]);
		}


		//输出匹配结果
		double Max = 0;
		char tmpch;

		vector<pair<char, double>> imfVec;
		vector<pair<char, double>> whVec;
		for (auto w : matchImform)
		{
			if (w.second > Max)
			{
				Max = w.second;
				tmpch = w.first;
			}
			imfVec.push_back(make_pair(w.first, w.second));
		}
		sort(imfVec.begin(), imfVec.end(), cmp);

		pair<char, double> preinf('\0', 0.0);
		pair<char, double> prewh('\0', 0.0);
		int t = 0;
		for (auto w : imfVec)
		{
			if (preinf.second - w.second < 0)
			{
				preinf = make_pair(w.first, w.second);
				prewh = make_pair(w.first, WtoHArr[w.first]);
			}
			/*else if (preinf.second - w.second <= 0.02 && preinf.second - w.second >= 0)
			{
				double tmp = fabs(prewh.second - 1) - fabs(WtoHArr[w.first] - 1);
				if (tmp > 0)
				{
					preinf = make_pair(w.first, w.second);
					prewh = make_pair(w.first, WtoHArr[w.first]);
				}
			}*/
			else if (preinf.second == w.second)
			{
				if (fabs(prewh.second - 1) - fabs(WtoHArr[w.first] - 1) > 0)
				{
					preinf = make_pair(w.first, w.second);
					prewh = make_pair(w.first, WtoHArr[w.first]);
				}
			}
			
			
			//cout << w.first << "  " << w.second << "   wh:" << WtoHArr[w.first] << endl;
			t++;
			if (t == 7)
			{
				break;
			}
		}
		cout << preinf.first << "\t" << preinf.second << "\t" << prewh.second << endl;
		//int tt = 0;
		//for (auto w : imfVec)
		//{
			
		//}
		
		//cout << i << ":  " << tmpch << "   Max: " << Max << "   " << matchImform[tmpch] << "\tZ: " << matchImform['Z'] << "\twtohZ:" << WtoHArr['Z'] << "\tz: " << matchImform['z'] << "\twtohz" << WtoHArr['z'] << endl;

	}


	finish = clock();
	cout << "time:" << (double)(finish - start) / 1000 << "s" << endl;
	system("pause");
	waitKey(0);
	return 0;
}







/*_finddata_t fileDir;
string ddir = "E:\\graduationDesign\\font\\ArialNarrow\\";
char* dir = "E:\\graduationDesign\\font\\ArialNarrow\\*.*";
long lfDir;
string nameStr;
if ((lfDir = _findfirst(dir, &fileDir)) == -1l)
{
cout << "No file found" << endl;
}
else
{
do
{
if (fileDir.name[0] != '.')
{
cout << fileDir.name << endl;
nameStr = ddir + fileDir.name;
string tmpStr;
tmpStr = nameStr.substr(nameStr.length() - 4, nameStr.length() - 1);
if (tmpStr == ".jpg")
{
Mat imgRead = imread(nameStr);
getNormalTxt(imgRead, nameStr);
}
}
} while (_findnext(lfDir, &fileDir) == 0);
}
//Mat imgRead = imread(nameStr);
//Mat imgRead2 = getMode(imgRead, nameStr);
//getNormalTxt(imgRead, nameStr);
//imshow("sd", imgRead);
_findclose(lfDir);
waitKey(0);
system("pause");
return 0;*/

