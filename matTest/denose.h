#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <stack>
#include <vector>

#ifndef _NAMEFUNC_H_
#include "nameFunc.h"
#endif

using namespace std;
using namespace cv;

int parent[2000] = { 0 };

int find(int x, int parent[])
{
	int i = x;
	while (0 != parent[i])
		i = parent[i];
	return i;
}

void union_label(int x, int y, int parent[])
{
	int i = x;
	int j = y;
	while (0 != parent[i])
		i = parent[i];
	while (0 != parent[j])
		j = parent[j];
	if (i != j)
	{
		if (i < j)
			parent[j] = i;
		else
			parent[i] = j;
	}
}

/*void smooth(Mat& image)
{
	int nl = image.rows;
	int nc = image.cols * image.channels();
	
	for (int i = 1; i < nl - 1; i++)
	{
		uchar* data_up = image.ptr<uchar>(i - 1);
		uchar* data = image.ptr<uchar>(i);
		uchar* data_down = image.ptr<uchar>(i + 1);
		for (int j = 1; j < nc - 1; j++)
		{
			if (data[j] == 0)
			{
				if (data[j - 1] != 0 && data[j + 1] != 0)
					data[j] = 255;
				if (data_up[j] != 0 && data_down[j] != 0)
					data[j] = 255;
			}
		}
	}
}*/

Mat getDenose(Mat &binImg, string nameStr, Mat& label)
{
	label.create(binImg.size(), binImg.type());
	binImg.convertTo(label, CV_32SC1);

	int nl = binImg.rows;
	int nc = binImg.cols * binImg.channels();
	int label_value = 0;

	for (int i = 0; i < nl; i++)
	{
		uchar* data = binImg.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] <= 100)
				data[j] = 0;
			else
				data[j] = 255;
		}
	}

	int* init_one_label = label.ptr<int>(0);//初始化第一行
	for (int j = 0; j < nc; j++)
		init_one_label[j] = 0;
	for (int i = 1; i < nl; i++)//第一次扫描
	{
		uchar* p_binary = binImg.ptr<uchar>(i);
		uchar* p_binary_up = binImg.ptr<uchar>(i - 1);
		int* p_label = label.ptr<int>(i);
		int* p_label_up = label.ptr<int>(i - 1);
		for (int j = 0; j < nc; j++)
		{
			p_label[j] = 0;
		}

		for (int j = 1; j < nc; j++)
		{
			if (0 == p_binary[j])
			{
				if (255 == p_binary[j - 1] && 255 == p_binary_up[j])
				{
					label_value++;
					p_label[j] = (int)label_value;
					//cout << (int)p_label[j] << endl;
				}
				else if (0 == p_binary[j - 1] && 255 == p_binary_up[j])
				{
					p_label[j] = p_label[j - 1];
				}
				else if (255 == p_binary[j - 1] && 0 == p_binary_up[j])
				{
					p_label[j] = p_label_up[j];
				}
				else if (0 == p_binary[j - 1] && 0 == p_binary_up[j])
				{
					p_label[j] = p_label[j - 1] <= p_label_up[j] ? p_label[j - 1] : p_label_up[j];
					union_label(p_label[j - 1], p_label_up[j], parent);
				}
			}
		}
	}

	for (int i = 1; i < nl - 1; i++)//第二次扫描
	{
		int* p_label = label.ptr<int>(i);
		for (int j = 1; j < nc - 1; j++)
		{
			if (p_label[j] > 0)
			{
				p_label[j] = find(p_label[j], parent);
			}
		}
	}


	int pixel_mark_num[2000] = { 0 };
	memset(pixel_mark_num, 0, 2000);
	int noise[2000] = { 0 }, tmp = 0;
	memset(noise, 0, 2000);

	for (int i = 1; i < nl; i++)
	{
		int* data = label.ptr<int>(i);
		for (int j = 1; j < nc; j++)
		{
			if (data[j] != 0)
			{
				pixel_mark_num[data[j]]++;
			}
		}
	}

	for (int i = 1; i < 2000; i++)
	{
		if (pixel_mark_num[i] > 0 && pixel_mark_num[i] <= 7)//目前将25像素一下的点当成噪点去掉
		{
			noise[tmp] = i;
			tmp++;
		}
	}

	for (int l = 1; l < tmp; l++)
	{
		int tmpLoca = noise[l];
		for (int i = 1; i < nl; i++)
		{
			int* data_label = label.ptr<int>(i);
			uchar* data_imageSplit = binImg.ptr<uchar>(i);
			for (int j = 1; j < nc; j++)
			{
				if (data_label[j] == tmpLoca)
					data_imageSplit[j] = 255;
			}
		}
	}
	

	string add_str = "denose.jpg";
	string res = getPicName(nameStr, add_str, 4);
	//imshow("donosePic", binImg);
	//waitKey(0);
	cv::imwrite(res, binImg);
	return binImg;
}

