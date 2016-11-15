#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <math.h>
#include <time.h>
#ifndef _NAMEFUNC_H_
#include "nameFunc.h"
#endif
using namespace std;
using namespace cv;



vector<Point> getPoints(Mat& image)
{
	int nl = image.rows;
	int nc = image.cols * image.channels();
	vector<Point>points;

	for (int i = 0; i < nl; i++)
	{
		uchar* data = image.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] == 255)
				points.push_back(Point(j, i));
		}
	}
	return points;
}

double getLine(Mat& image)
{
	Vec4f line;
	vector<Point>points = getPoints(image);
	fitLine(Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);
	//cout << "line: (" << line[0] << "," << line[1] << ")(" << line[2] << "," << line[3] << ")\n";
	double k = line[1] / line[0];
	double step = 40;
	cv::line(image, Point(line[2] - step, line[3] - k * step), Point(line[2] + step, line[3] + k * step), Scalar(100));
	double angle = atan(k) * 180 / 3.1415;
	cout << k << " " << angle << endl;
	return angle;
}

void imageRotat(Mat& imgIn, string nameStr)
{
	Mat imgOut;
	double angle = getLine(imgIn);
	int oldWidth = imgIn.cols;
	int oldHeight = imgIn.rows;

	cout << imgIn.channels() << endl;
	Point center = Point(imgIn.cols / 2, imgIn.rows / 2);
	Mat rot_mat(2, 3, CV_32FC1);
	rot_mat = getRotationMatrix2D(center, angle, 1.0);//角度不理想，先设为0；

	cout << angle / 38.0 << endl;
	warpAffine(imgIn ,imgOut, rot_mat, imgIn.size(), 1, 0, Scalar(255));


	imshow("sss", imgIn);
	imshow("rotatPic", imgOut);
	char* add_str = "rotat.jpg";
	string res = getPicName(nameStr, add_str, 12);
	//imwrite(res, imgOut);
	waitKey(0);
	//cout << res << endl;
}

int getEmptyLine(Mat& image)//获取图像中空的行数
{
	int nl = image.rows;
	int nc = image.cols * image.channels();
	int n = 0;

	
	for (int i = 0; i < nl; i++)
	{
		int tmpn = 0;
		int flag = 0;
		uchar* data = image.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] == 0)
			{
				flag = 1;
				break;
			}
		}
		if (!flag)
			n++;
	}
	return n;
}

Mat imgRotatTwo(Mat& src, float angle)
{
	Mat dst;
	int oldWidth = src.cols;
	int oldHeight = src.rows;
	//原图像的四个角坐标，以图像中心为原点
	float srcX1 = (float)(-oldWidth / 2);
	float srcY1 = (float)(oldHeight / 2);
	float srcX2 = (float)(oldWidth / 2);
	float srcY2 = (float)(oldHeight / 2);
	float srcX3 = (float)(-oldWidth / 2);
	float srcY3 = (float)(-oldHeight / 2);
	float srcX4 = (float)(oldWidth / 2);
	float srcY4 = (float)(-oldHeight / 2);

	float dstX1 = (float)(srcX1 * cos(angle) + srcY1 * sin(angle));
	float dstY1 = (float)(-srcX1 * sin(angle) + srcY1 * cos(angle));
	float dstX2 = (float)(srcX2 * cos(angle) + srcY2 * sin(angle));
	float dstY2 = (float)(-srcX2 * sin(angle) + srcY2 * cos(angle));
	float dstX3 = (float)(srcX3 * cos(angle) + srcY3 * sin(angle));
	float dstY3 = (float)(-srcX3 * sin(angle) + srcY3 * cos(angle));
	float dstX4 = (float)(srcX4 * cos(angle) + srcY4 * sin(angle));
	float dstY4 = (float)(-srcX4 * sin(angle) + srcY4 * cos(angle));

	int newWidth = max(fabs(dstX4 - dstX1), fabs(dstX3 - dstX2));
	int newHeight = max(fabs(dstY4 - dstY1), fabs(dstY3 - dstY2));

	dst.create(newHeight, newWidth, src.type());

	float dx = -0.5 * newWidth * cos(angle) - 0.5 * newHeight * sin(angle) + 0.5 * oldWidth;
	float dy = 0.5 * newWidth * sin(angle) - 0.5 * newHeight * cos(angle) + 0.5 * oldHeight;

	int x, y;
	for (int i = 0; i < newHeight; i++)
	{
		for (int j = 0; j < newWidth; j++)
		{
			x = float(j) * cos(angle) + float(i) * sin(angle) + dx;
			y = float(-j) * sin(angle) + float(i) * cos(angle) + dy;

			if ((x < 0) || (x >= oldWidth) || (y < 0) || (y >= oldHeight))
			{
				if (src.channels() == 3)
				{
					dst.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
				}
				else if (src.channels() == 1)
				{
					dst.at<uchar>(i, j) = 255;
				}
			}
			else
			{
				if (src.channels() == 3)
				{
					dst.at<Vec3b>(i, j) = src.at<Vec3b>(y, x);
				}
				else if (src.channels() == 1)
				{
					dst.at<uchar>(i, j) = src.at<uchar>(y, x);
				}
			}
		}
	}

	return dst;
}

Mat getBestAngleImg(Mat& img, string nameStr)
{
	int emptyLineArr[30] = { 0 };
	Mat tmpImg = img;
	int tmp = 0;
	for (float i = -3.0f; i <= 3.0f; i += 0.25f)
	{
		tmpImg = imgRotatTwo(img, i * 3.1415926 / 180.0f);
		emptyLineArr[tmp] = getEmptyLine(tmpImg);
		//cout << emptyLineArr[tmp] << endl;
		tmp++;
	}
	
	int lineMax = 0;
	int flagAngle = 0;
	for (int i = 0; i < tmp; i++)
	{
		//cout << emptyLineArr[i] << endl;
		if (emptyLineArr[i] >= lineMax)
		{
			lineMax = emptyLineArr[i];
			flagAngle = i;
		}
	}

	float finAngle = -3.0f + 0.25f * flagAngle;
	tmpImg = imgRotatTwo(img, finAngle * 3.1415926 / 180.0f);
	//cout << flagAngle << " " << lineMax << endl;
	//imshow("roat", tmpImg);
	//waitKey(0);
	char* add_str = "rotat.jpg";
	string res = getPicName(nameStr, add_str, 4);
	//cout << res << endl;
	imwrite(res, tmpImg);
	return tmpImg;
}