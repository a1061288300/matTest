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
#include "matchFun.h"
//#include "mess.h"
using namespace std;
using namespace cv;

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
		//cout << "start" << endl;
		getMatchWordTxt(imageRead6, words[k].up_line, words[k].down_line, words[k].left_line, words[k].right_line, name);
	}



	
	
	getModeNorNameArr(modePreName, modeNorNameArr, modeImformNameArr);
	getMatchNameArr(matchPreName, matchNameArr);
	
	for (int i = 0; i < wordsNum; i++)
	{
		
		for (int j = 0; modeNorNameArr[j] != ""; j++)
		{
			getWtoHArr(words[i], matchNameArr[i], modeImformNameArr[j]);
		}
	}

	pixelMatch(WtoHArr, modeNorNameArr, matchNameArr, modeImformNameArr);
	//horizonVertical(modeNorNameArr);

	finish = clock();
	cout << "time:" << finish - start << "ms" << endl;
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

