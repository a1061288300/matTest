#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <string>

using namespace std;
using namespace cv;

Mat splitFontPic(Mat& img)//抽取单通道
{
	Mat splitImg;
	vector<Mat> chann;
	split(img, chann);
	splitImg = chann.at(0);
	return splitImg;
}

Mat beBinary(Mat& img)//二值化
{
	Mat imgs = splitFontPic(img);
	int nl = imgs.rows;
	int nc = imgs.cols * imgs.channels();
	for (int i = 0; i < nl; i++)
	{
		uchar* data = imgs.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] != 255)
				data[j] = 0;
		}
	}
	return imgs;
}

Mat wipeEmpty(Mat& img)//将上下左右没用的空白去除
{
	Mat imgs = beBinary(img);
	int nl = imgs.rows;
	int nc = imgs.cols * imgs.channels();
	
	int lineBlackNumi[100] = { 0 };
	int lineBlackNumj[100] = { 0 };
	for (int i = 0; i < nl; i++)
	{
		uchar* data = imgs.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] == 0)
			{
				lineBlackNumi[i]++;
				lineBlackNumj[j]++;
			}
		}
	}

	int x1 = 0, y1 = 0, x2 = nl - 1, y2 = nc - 1, width = 0, height = 0;
	while (lineBlackNumi[x1] == 0 && x1 < nl)
		x1++;
	while (lineBlackNumj[y1] == 0 && y1 < nc)
		y1++;
	while (lineBlackNumi[x2] == 0 && x2 > 0)
		x2--;
	while (lineBlackNumj[y2] == 0 && y2 > 0)
		y2--;
	width = y2 - y1;
	height = x2 - x1;

	cout << nl << " " << nc << endl;
	cout << x1 << " " << y1 << " " << x2 << " " << y2 << " " << width << " " << height << endl;
	rect = Rect(y1, x1, width, height);
	imgs(rect).copyTo(dst);
	return dst;
}

void fontPixelToTxt(Mat& img, string str)
{
	ofstream outPut;
	outPut.open(str, ios::out | ios::trunc);
	int nl = img.rows;
	int nc = img.cols * img.channels();

	for (int i = 0; i < nl; i++)
	{
		uchar* data = img.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] == 0)
				outPut << "1";
			else
				outPut << "0";
		}
		outPut << "\n";
	}
}

string getNameString(string nameStr)
{
	string add_string = ".txt";
	nameStr = nameStr.substr(0, nameStr.length() - 4);
	nameStr += add_string;
	return nameStr;
}

void getImformTxt(Mat& img, string nameStr)
{
	int width = img.cols;
	int height = img.rows;
	double wdivh = (double)width / (double)height;

	ofstream outPut;
	string name = "imform.txt";
	nameStr = nameStr.substr(0, nameStr.length() - 4);
	nameStr += name;
	outPut.open(nameStr, ios::out | ios::trunc);
	outPut << wdivh << endl << width << endl << height << endl;
	outPut.close();
}

Mat getMode(Mat& img, string nameStr)
{
	Mat imgS = wipeEmpty(img);

	//imwrite(nameStr, imgS);

	nameStr = getNameString(nameStr);
	fontPixelToTxt(imgS, nameStr);

	return imgS;
}

void getNormalTxt(Mat& img, string nameStr)
{

	Mat img1 = splitFontPic(img);
	getImformTxt(img1, nameStr);


	Size dsize = Size(50, 50);
	Mat img2 = Mat(dsize, CV_32S);
	resize(img1, img2, dsize);
	//imshow("resize", img2);
	//cout << img.cols << " " << img.rows << endl;
	string name = "nor.txt";
	nameStr = nameStr.substr(0, nameStr.length() - 4);
	nameStr += name;
	ofstream outPut;
	outPut.open(nameStr, ios::out | ios::trunc);
	//imshow("22", img2);
	int nl = img2.rows;
	int nc = img2.cols * img2.channels();
	cout << img1.channels();
	for (int i = 0; i < nl; i++)
	{
		uchar* data = img2.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] <= 100)
				outPut << "1";
			else
				outPut << "0";
		}
		outPut << "\n";
	}
	outPut.close();
	waitKey(0);
}