#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
#ifndef _NAMEFUNC_H_
#include "nameFunc.h"
#endif
using namespace std;
using namespace cv;

Mat GrayIma(Mat& imageRead, string nameStr)
{
	int nl = imageRead.rows;
	int nc = imageRead.cols * imageRead.channels();

	for (int i = 0; i < nl; i++)
	{
		uchar* data = imageRead.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (j % 3 == 0)
				data[j] = data[j] * 0.11 + data[j + 1] * 0.59 + data[j + 2] * 0.3;
			else
				data[j] = data[j - j % 3];
		}
	}

	Mat splitImg;
	vector<Mat> chan;
	split(imageRead, chan);
	splitImg = chan.at(0);

	string add_str = "gray.jpg";
	string res = getPicName(nameStr, add_str, 4);
	imwrite(res, splitImg);
	return splitImg;
}