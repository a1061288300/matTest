#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
using namespace std;
using namespace cv;

void getMatchWordTxt(Mat& img, int up, int down, int left, int right, string nameStr)//将待匹配区域以50*50的方式存在txt
{
	Mat dst;
	rect = Rect(left, up, right - left, down - up);
	img(rect).copyTo(dst);
	Size dsize = Size(10, 10);
	Mat dst2 = Mat(dsize, CV_32S);
	resize(dst, dst2, dsize);

	//imshow("dst2", dst2);
	ofstream outPut;
	outPut.open(nameStr, ios::out | ios::trunc);
	int nl = dst2.rows;
	int nc = dst2.cols * dst2.channels();
	if (outPut.is_open())
	{
		for (int i = 0; i < nl; i++)
		{
			uchar* data = dst2.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				if (data[j] <= 100)
					outPut << "1";
				else
					outPut << "0";
			}
			outPut << endl;
		}
	}

	outPut.close();
	waitKey(0);
}
