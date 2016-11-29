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

#ifndef _CUTTEST_H_
#include "cutTest.h"
#endif

#ifndef _MATCH_H_
#include "match.h"
#endif

using namespace std;
using namespace cv;

string modeNorNameArr[500];
string modeImformNameArr[500];
string matchNameArr[500];

int cmp(pair<char, double>& x, pair<char, double>& y)
{
	return x.second > y.second;
}

void pixelMatch(map<char, double>WtoHArr, string modeNorNameArr[], string matchNameArr[], string modeImformNameArr[])//通过像素匹配和宽高匹配的结果
{
	for (int i = 0; i < wordsNum; i++)
	{
		if ((i >= 25 && i <= 34) || (i >= 44 && i <= 45) || (i >= 69 && i <= 73) || (i >= 77 && i <= 83) || (i >= 91 && i <= 94) || (i >= 100 && i <= 110))
		for (int j = 0; modeNorNameArr[j] != ""; j++)
		{
			getMatchArr(matchNameArr[i], modeNorNameArr[j]);
			//getWtoHArr(words[i], matchNameArr[i], modeImformNameArr[j]);
		}
		else
			continue;


		//输出匹配结果
		double Max = 0;
		char tmpch;

		vector<pair<char, double>> imfVec;
		//vector<pair<char, double>> whVec;
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
		cout << i << "\t" << preinf.first << "\t" << preinf.second << "\t" << prewh.second << endl;
		//int tt = 0;
		//for (auto w : imfVec)
		//{

		//}

		//cout << i << ":  " << tmpch << "   Max: " << Max << "   " << matchImform[tmpch] << "\tZ: " << matchImform['Z'] << "\twtohZ:" << WtoHArr['Z'] << "\tz: " << matchImform['z'] << "\twtohz" << WtoHArr['z'] << endl;

	}
}


void horizonVertical(string modeNorNameArr[])//使用投影区块匹配方法
{	
	for (int i = 0; i < wordsNum; i++)
	{
		if ((i >= 25 && i <= 34) || (i >= 44 && i <= 45) || (i >= 69 && i <= 73) || (i >= 77 && i <= 83) || (i >= 91 && i <= 94) || (i >= 100 && i <= 110))
			getMatchHorVerArr(matchNameArr[i]);
		else
			continue;
		double tmpMax = 0.0;
		char tmpch;
		for (int j = 0; modeNorNameArr[j] != ""; j++)
		{
			getModeHorVerArr(modeNorNameArr[j]);
			double res = getHVArr(modehorizon, modevertical, matchhorizon, matchvertical);

			if (res > tmpMax)
			{
				tmpMax = res;
				tmpch = (char)(getNorNumber(modeNorNameArr[j]));
			}
		}
		cout << i << " " << tmpch << " " << tmpMax << endl;
	}
}
