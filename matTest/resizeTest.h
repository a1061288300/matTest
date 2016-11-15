#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;


Mat resizeImage(Mat& imageRead, string nameStr)
{
	double scaleCols, scaleRows;
	scaleCols = 600 / (double)imageRead.cols;
	scaleRows = 600 / (double)imageRead.rows;
	Size dsize = Size(imageRead.cols * scaleCols, imageRead.rows * scaleRows);
	Mat imageRead2 = Mat(dsize, CV_32S);
	resize(imageRead, imageRead2, dsize);

	string add_str = "resize.jpg";
	string res = getPicName(nameStr, add_str, 4);
	imwrite(res, imageRead2);
	return imageRead2;
}