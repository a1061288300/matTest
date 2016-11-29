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

bool IsDimodal(double HistGram[])//�ж��Ƿ�Ϊ����
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

int twoPeakDifference = 0;//��¼��һ����ֵ�͵ڶ�����ֵ�Ĳ�ֵ��peak1 - peak2

int GetLowestPixel(int Histgram[])//��ùȵ�����ֵ
{
	double HistGramC[256];
	double HistGramCC[256];
	for (int i = 0; i < 256; i++)
	{
		HistGramC[i] = Histgram[i];
		HistGramCC[i] = Histgram[i];
	}

	//ͨ��������ƽ��ֵ��ʹ����ƽ��
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
			return -1;//�޷���Ϊƽ��
	}

	bool PeakFound = false;//Ѱ�Ҳ���
	int fPeak = 0, sPeak = 0;
	for (int i = 1; i < 255; i++)
	{
		if (HistGramC[i] > HistGramC[i - 1] && HistGramC[i] > HistGramC[i + 1])
		{
			PeakFound = true;
			fPeak = HistGramC[i];
		}
		if (PeakFound && HistGramC[i] <= HistGramC[i - 1] && HistGramC[i] <= HistGramC[i + 1])//�ҵ���һ�㲨�壬���Ĳ��ȼ�Ϊ����
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

Mat getBinaryImage(Mat& imageRead, string nameStr)//���ж�ֵ��
{
	int nl = imageRead.rows;
	int nc = imageRead.cols * imageRead.channels();

	int HistGram[500];
	for (int i = 0; i < nl; i++)//ͳ��;����������   ÿ����ɫ�ĸ���
	{
		uchar* data = imageRead.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			HistGram[data[j]]++;
		}
	}

	int tmpPixel = GetLowestPixel(HistGram);//��������ֵ
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