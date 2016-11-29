#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#ifndef _NAMEFUNC_H_
#include "nameFunc.h"
#endif
using namespace std;
using namespace cv;

bool IsDimodal(double HistGram[])//判断是否为单峰
{
	int count = 0;
	for (int i = 1; i < 255; i++)
	{
		if (HistGram[i - 1] < HistGram[i] && HistGram[i] > HistGram[i + 1])
		{
			count++;
			if (count > 2)
				return false;
		}
	}
	if (count == 2)
		return true;
	else
		return false;
}

int twoPeakDifference = 0;//记录第一个峰值和第二个峰值的差值，peak1 - peak2

int GetLowestPixel(int Histgram[])//获得谷底像素值
{
	double HistGramC[256];
	double HistGramCC[256];
	for (int i = 0; i < 256; i++)
	{
		HistGramC[i] = Histgram[i];
		HistGramCC[i] = Histgram[i];
	}

	//通过三点求平均值来使曲线平滑
	int num = 0;
	while (!IsDimodal(HistGramCC))
	{
		HistGramCC[0] = (HistGramC[0] + HistGramC[0] + HistGramC[1]) / 3;
		HistGramCC[255] = (HistGramC[254] + HistGramC[255] + HistGramC[255]) / 3;
		for (int i = 1; i < 255; i++)
		{
			HistGramCC[i] = (HistGramC[i - 1] + HistGramC[i] + HistGramC[i + 1]) / 3;
		}

		for (int i = 0; i < 256; i++)
		{
			HistGramC[i] = HistGramCC[i];
		}
		num++;
		if (num > 1000)
			return -1;//无法变为平滑
	}

	bool PeakFound = false;//寻找波谷
	int fPeak = 0, sPeak = 0;
	for (int i = 1; i < 255; i++)
	{
		if (HistGramC[i] > HistGramC[i - 1] && HistGramC[i] > HistGramC[i + 1])
		{
			PeakFound = true;
			fPeak = HistGramC[i];
		}
		if (PeakFound && HistGramC[i] <= HistGramC[i - 1] && HistGramC[i] <= HistGramC[i + 1])//找到低一点波峰，随后的波谷即为所求
		{
			for (int j = i + 1; j < 255; j++)
			{
				if (HistGramC[j] > HistGramC[j - 1] && HistGramC[j] > HistGramC[j + 1])
				{
					sPeak = HistGramC[j];
				}
				twoPeakDifference = fPeak - sPeak;
			}
			return i;
		}
	}
	return -1;
}

Mat getBinaryImage(Mat& imageRead, string nameStr)//进行二值化
{
	int nl = imageRead.rows;
	int nc = imageRead.cols * imageRead.channels();

	int HistGram[500];
	for (int i = 0; i < nl; i++)//统计途中所有像素   每个颜色的个数
	{
		uchar* data = imageRead.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			HistGram[data[j]]++;
		}
	}

	int tmpPixel = GetLowestPixel(HistGram);//波谷像素值
	for (int i = 0; i < nl; i++)
	{
		uchar* data = imageRead.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if ((data[j] <= tmpPixel && twoPeakDifference < 0) || (data[j] > tmpPixel && twoPeakDifference > 0))
			{
				data[j] = 0;
			}
			else
			{
				data[j] = 255;
			}
		}
	}

	/*for (int i = 0; i < nl; i++)
	{
		uchar* data = imageRead.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] != 0)
				data[j] = 255;
		}
	}*/

	string add_str = "binary.jpg";
	string res = getPicName(nameStr, add_str, 4);
	imwrite(res, imageRead);
	return imageRead;
}