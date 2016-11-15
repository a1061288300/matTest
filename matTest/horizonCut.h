#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

int horizonPixelNum[1000] = { 0 };
int horizonPixelNumC[1000] = { 0 };
int cutHorizonLineNum[100] = { 0 };
int cutVerticalLineNum[100][100] = { 0 };
int cutParent[1000] = { 0 };
struct wordImformation//存储字符的相关信息
{
	int line_num = 0;//处于第几行
	int widthPixel = 0;
	int heightPixel = 0;
	double hwScale = 0.0;
	int up_line = 0;
	int down_line = 0;
	int left_line = 0;
	int right_line = 0;
}words[1000];
int wordsNum = 0;

int find1(int x, int parent[])
{
	int i = x;
	while (0 != parent[i])
		i = parent[i];
	return i;
}

void union_label1(int x, int y, int parent[])
{
	int i = x;
	int j = y;
	while (0 != parent[i])
		i = parent[i];
	while (0 != parent[j])
		j = parent[j];
	if (i != j)
		parent[i] = j;
}


Mat unicom_area(Mat& binary)
{
	Mat label;
	label.create(binary.size(), binary.type());
	int label_value = 0;
	int nl = binary.rows;
	int nc = binary.cols * binary.channels();

	for (int i = 1; i < nl; i++)
	{
		uchar* p_binary = binary.ptr<uchar>(i);
		uchar* p_binary_up = binary.ptr<uchar>(i - 1);
		uchar* p_label = label.ptr<uchar>(i);
		uchar* p_label_up = label.ptr<uchar>(i - 1);

		for (int j = 0; j < nc; j++)
			p_label[j] = 0;

		for (int j = 1; j < nc; j++)
		{
			if (0 == p_binary[j])
			{
				if (255 == p_binary[j - 1] && 255 == p_binary_up[j])
				{
					label_value++;
					p_label[j] = (int)label_value;
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
					union_label1(p_label[j - 1], p_label_up[j], cutParent);
				}
			}
		}
	}

	for (int i = 1; i < nl - 1; i++)
	{
		uchar* p_label = label.ptr<uchar>(i);
		for (int j = 1; j < nc - 1; j++)
		{
			if (p_label > 0)
				p_label[j] = find1(p_label[j], cutParent);
		}
	}
	return label;
}

void getHorizonCutLineNum(Mat& imageRead)
{
	int nl = imageRead.rows;
	int nc = imageRead.cols * imageRead.channels();

	for (int i = 0; i < nl; i++)
	{
		uchar* data = imageRead.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (data[j] <= 100)
			{
				horizonPixelNum[i]++;
			}
		}
		//cout << horizonPixelNum[i] << " ";

	}

	//求相邻三点平均值使曲线变的平滑
	horizonPixelNumC[0] = horizonPixelNum[0];
	horizonPixelNumC[nl - 1] = horizonPixelNum[nl - 1];
	for (int i = 1; i < nl - 1; i++)
	{
		horizonPixelNumC[i] = (horizonPixelNum[i - 1] + horizonPixelNum[i] + horizonPixelNum[i + 1]) / 3;
	}

	//值小于50的当做背景
	int cutLine_tmp = 0;
	for (int i = 1; i < nl; i++)
	{
		if (horizonPixelNumC[i] < 50 && horizonPixelNumC[i + 1] >= 50)
		{
			cutHorizonLineNum[cutLine_tmp] = i + 1;
			if (cutLine_tmp && !(cutLine_tmp % 2) && cutHorizonLineNum[cutLine_tmp] - cutHorizonLineNum[cutLine_tmp - 1] <= 10)//两行之间的空隙小于10像素,可能是i，j之类的字符，要连接起来
			{
				cutLine_tmp--;
				continue;
			}
			cutLine_tmp++;
		}
		else if (horizonPixelNumC[i] >= 50 && horizonPixelNumC[i + 1] < 50)
		{
			cutHorizonLineNum[cutLine_tmp] = i;
			cutLine_tmp++;
		}
	}
}

wordImformation* projectionToLine(Mat& image)//垂直方向投影 返回一个包含列分割线的数组
{
	int nl = image.rows;
	int nc = image.cols * image.channels();
	int cutVerticalLineNum[100] = { 0 };
	int tmpNum = 0;
	int tmp_up = 0, tmp_down = 0;
	int pixelNum[1000] = { 0 };//用于记录该列中属于字符的像素的个数

	for (int horizon_i = 0; cutHorizonLineNum[horizon_i] != 0; horizon_i++)
	{
		if (horizon_i % 2 == 0)
		{
			tmp_up = horizon_i;
			tmp_down = horizon_i + 1;
			horizon_i++;
			memset(cutVerticalLineNum, 0, 100);
			for (int j = 1; j < nc; j++)
			{
				for (int i = tmp_up; i <= tmp_down; i++)
				{
					uchar* data = image.ptr<uchar>(i);
					if (data[j] != 0)
					{
						pixelNum[j]++;
					}
				}
				if (pixelNum[j] == 1 && pixelNum[j - 1] == 0)
				{
					continue;
				}
				else if (pixelNum[j] > 1 && pixelNum[j - 1] == 0)
				{
					cutVerticalLineNum[tmpNum] = j;
					tmpNum++;
				}
				else if (pixelNum[j] == 0 && pixelNum[j - 1] > 1)
				{
					cutVerticalLineNum[tmpNum] = j - 1;
					tmpNum++;
				}
				else if (pixelNum[j] == 1 && pixelNum[j - 1] > 1)
				{
					cutVerticalLineNum[tmpNum] = j - 1;
					tmpNum++;
				}
			}
		}
		for (int k = 0; k < tmpNum; k++)
		{
			if (k % 2 == 0)
			{
				words[wordsNum].up_line = tmp_up;
				words[wordsNum].down_line = tmp_down;
				words[wordsNum].left_line = k;
				words[wordsNum].right_line = k + 1;
				words[wordsNum].line_num = horizon_i;
			}
		}
	}
	return words;
}

wordImformation compressionWords(Mat& image, wordImformation word)//将一个字符的上下无用空间进行压缩,并记录字符长宽比例，注意“.”字符
{
	int tmp_horizon_up = 0, tmp_horizon_down = 0, tmp_vertical_up = 0, tmp_vertical_down = 0;
	for (int i_up = word.up_line; i_up < word.down_line; i_up++)
	{
		tmp_horizon_up = word.up_line;
		int flag_up = 0;
		uchar* data_up = image.ptr<uchar>(i_up);
		for (int j = word.left_line; j <= word.right_line; j++)
		{
			if (data_up[j] != 0)
				flag_up++;
		}
		if (flag_up == 0)
			tmp_horizon_up++;
		else if (flag_up != 0)
		{
			break;
		}
	}
	for (int i_down = word.down_line; i_down > word.up_line; i_down--)
	{
		tmp_horizon_down = word.down_line;
		int flag_down = 0;
		uchar* data_down = image.ptr<uchar>(i_down);
		for (int j = word.left_line; j <= word.right_line; j++)
		{
			if (data_down[j] != 0)
				flag_down++;
		}
		if (flag_down == 0)
			tmp_horizon_down--;
		else if (flag_down != 0)
			break;
	}
	word.widthPixel = word.right_line - word.left_line;
	word.down_line = tmp_horizon_down;
	word.up_line = tmp_horizon_up;
	word.heightPixel = tmp_horizon_down - tmp_horizon_up;
	word.hwScale = word.widthPixel - word.heightPixel;
	return word;
}

void InitWords()
{
	
}