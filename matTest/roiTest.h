#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <math.h>
#include <cxcore.h>
#ifndef _NAMEFUNC_H_
#include "nameFunc.h"
#endif
using namespace std;
using namespace cv;

bool drawing = false;
Rect rect(-1, -1, -1, -1);
Point orign(-1, -1);
Mat src;
string windowName = "mouseRoiPic";
string res;
Mat dst;

void onMouseEvent(int event, int x, int y, int flag, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN && !CV_EVENT_MOUSEMOVE)
	{
		drawing = true;
		orign = Point(x, y);
		rect = Rect(x, y, 0, 0);
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flag && CV_EVENT_FLAG_LBUTTON))
	{
		if (drawing)
		{
			rect.x = min(orign.x, x);
			rect.y = min(orign.y, y);
			rect.width = abs(orign.x - x);
			rect.height = abs(orign.y - y);
		}
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		drawing = false;
		if (rect.height == 0 || rect.width == 0)
		{
			cout << "ÇëÑ¡ÔñÒ»¿éÇøÓò" << endl;
		}
		else
		{
			rectangle(src, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(0), 1, 1, 0);
			destroyWindow(windowName);
			rect.x += 1;
			rect.y += 1;
			rect.width -= 1;
			rect.height -= 1;
			src(rect).copyTo(dst);
			imwrite(res, dst);
			imshow("dst", dst);
			imshow(windowName, src);
			waitKey(0);

		}

		//destroyWindow("dst");
		
	}
}

Mat getMouseRoi(Mat& image, string nameStr)
{
	src = image;
	string add_str = "mouseRoi.jpg";
	res = getPicName(nameStr, add_str, 4);
	namedWindow(windowName, 1);
	setMouseCallback(windowName, onMouseEvent, NULL);
	imshow("mouseRoiPic", src);
	waitKey(0);
	return dst;
}

Mat& getRoi(Mat& image, string nameStr)
{
	Mat Dst;
	rect = Rect(43, 52, 509, 247);
	rectangle(src, Point(43, 52), Point(552, 299), Scalar(0), 1, 1, 0);
	image(rect).copyTo(dst);
	//imshow("roi", dst);
	//waitKey(0);
	string add_str = "roi.jpg";
	res = getPicName(nameStr, add_str, 4);
	imwrite(res, dst);
	return dst;
}