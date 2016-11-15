#include <iostream>
#include <fstream>
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

void getPixelTxt(Mat& imageRead)
{
	ofstream outPut;
	outPut.open("E:\\graduationDesign\\image\\275\\275.txt", ios::out | ios::trunc);
	int nl = imageRead.rows;
	int nc = imageRead.cols * imageRead.channels();
	cout << "nl:" << nl << endl << "nc:" << nc << endl << "cols:" << imageRead.cols << endl << "channels:" << imageRead.channels() << endl;
	int count = 0;
	if (outPut.is_open())
	{
		for (int i = 0; i < nl; i++)
		{
			uchar* data = imageRead.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				if (data[j] == 0)
				{
					outPut << "1";
					count++;
				}
				else
				{
					outPut << "0";
				}
			}
			outPut << "\n";
		}
		cout << count << endl;
	}
	else
		cout << "failur" << endl;

	//imshow("sds", imageRead);

	outPut.close();
	waitKey(0);
}