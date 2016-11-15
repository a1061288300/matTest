#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
using namespace std;
using namespace cv;

struct wordImformation//存储字符的相关信息
{
	int line_num = 0;//处于第几行
	int widthPixel = 0;
	int heightPixel = 0;
	double whScale = 0.0;
	int up_line = 0;
	int down_line = 0;
	int left_line = 0;
	int right_line = 0;
}words[1000];
int wordsNum = 0;

int cutParent[2000] = { 0 };
int gHorizonCutNum[100] = { 0 };
int gVerticalCutNum[100] = { 0 };

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
	{
		if (i < j)
			parent[j] = i;
		else
			parent[i] = j;
	}
}

Mat unicom_area(Mat& binary)//求连通图
{
	Mat label;

	Mat imageSplit;
	vector<Mat> channel;
	split(binary, channel);
	imageSplit = channel.at(0);

	label.create(imageSplit.size(), imageSplit.type());
	imageSplit.convertTo(label, CV_32SC1);

	int label_value = 0;
	int nl = imageSplit.rows;
	int nc = imageSplit.cols * imageSplit.channels();

	for (int i = 0; i < nl; i++)
	{
		uchar* data = imageSplit.ptr<uchar>(i);
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

	//getPixelTxt(imageSplit);
	for (int i = 1; i < nl; i++)//第一次扫描
	{
		uchar* p_binary = imageSplit.ptr<uchar>(i);
		uchar* p_binary_up = imageSplit.ptr<uchar>(i - 1);
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
					union_label(p_label[j - 1], p_label_up[j], cutParent);
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
				p_label[j] = find1(p_label[j], cutParent);
			}
		}
	}

	/*for (int i = 0; i < 1000; i++)
	{
		cout << i << ":" << cutParent[i] << "\t";
		if (i % 10 == 0)
			cout << endl;
	}*/

	//std::cout << "unicom_area down!" << endl;

	return imageSplit;
	
}

void getHorizonCutLineNum(Mat& image)//获取水平分割线
{
	for (int i = 0; i < 100; i++)
		gHorizonCutNum[i] = 0;
	int horizonPixelNum[1000] = { 0 };
	int horizonPixelNumC[1000] = { 0 };
	int nl = image.rows;
	int nc = image.cols * image.channels();
	for (int i = 1; i < nl; i++)
	{
		uchar* data = image.ptr<uchar>(i);
		for (int j = 1; j < nc; j++)
		{
			if (data[j] == 0)
			{
				horizonPixelNum[i]++;
			}
			
		}
	}

	horizonPixelNumC[0] = horizonPixelNum[0];
	horizonPixelNumC[nl - 1] = horizonPixelNumC[nl - 1];
	for (int i = 1; i < nl - 1; i++)
	{
		horizonPixelNumC[i] = (horizonPixelNum[i - 1] + horizonPixelNum[i] + horizonPixelNum[i + 1]) / 3;
	}

	
	/*for (int i = 1; i < nl; i++)
	{
		cout << i <<":"<< horizonPixelNum[i] << "," << horizonPixelNumC[i] << "\t";
		if (i % 10 == 0)
			cout << endl;
	}
	cout << endl;*/


	int cutLine_tmp = 0;
	for (int i = 1; i < nl; i++)
	{
		if (horizonPixelNumC[i - 1] < 50 && horizonPixelNumC[i] >= 50)//50为界
		{
			gHorizonCutNum[cutLine_tmp] = i;
			//cout << "up" << ":" << gHorizonCutNum[cutLine_tmp] << endl;
			if (cutLine_tmp && !(cutLine_tmp % 2) && gHorizonCutNum[cutLine_tmp] - gHorizonCutNum[cutLine_tmp - 1] <= 15)
			{
				//cutLine_tmp--;
				//continue;
			}
			cutLine_tmp++;
		}
		if (horizonPixelNumC[i] < 50 && horizonPixelNumC[i - 1] >= 50)
		{
			gHorizonCutNum[cutLine_tmp] = i - 1;
			//cout << "down" << ":" << gHorizonCutNum[cutLine_tmp] << endl;
			cutLine_tmp++;
		}
	}
	//std::cout << "getHorizonCutLineNum down!" << endl;
}

void getVerticalCutLineNum(Mat& image, int line_up, int line_down)//获取竖直分割线
{
	for (int i = 0; i < 100; i++)
		gVerticalCutNum[i] = 0;
	int pixelNum[1000] = { 0 };
	int tmpNum = 0;
	int nc = image.cols * image.channels();

	for (int i = line_up; i <= line_down; i++)
	{
		uchar* data = image.ptr<uchar>(i);
		for (int j = 1; j < nc - 1; j++)
		{
			if (data[j] == 0)
				pixelNum[j]++;
		}
	}


	/*cout << line_up << "  " << line_down << endl;
	for (int i = 1; i < 300; i++)
	{
		cout << i << ":" << pixelNum[i] << "\t";
		if (i % 10 == 0)
			cout << endl;
	}
	cout << endl;*/

	for (int j = 1; j < nc - 1; j++)
	{
		if (pixelNum[j] > 1 && pixelNum[j - 1] <= 1)
		{
			gVerticalCutNum[tmpNum] = j;
			//cout << "left:" << gVerticalCutNum[tmpNum] << endl;
			tmpNum++;
		}
		else if (pixelNum[j] <= 1 && pixelNum[j - 1] > 1)
		{
			gVerticalCutNum[tmpNum] = j - 1;
			//cout << "right:" << gVerticalCutNum[tmpNum] << endl;
			tmpNum++;
		}
	}

	//std::cout << "getVerticalCutLineNum" << endl;
}

wordImformation* initWord(Mat& image, wordImformation words[])//对words赋值
{
	getHorizonCutLineNum(image);

	for (int i = 0; gHorizonCutNum[i] != 0; i++)
	{
		int line_up = 0, line_down = 0;
		for (int p = 0; p < 100; p++)
			gVerticalCutNum[p] = 0;
		//std::cout << "vertical ? " << endl;
		if (i % 2 == 0)
		{
			line_up = gHorizonCutNum[i];
			i++;
			line_down = gHorizonCutNum[i];
			getVerticalCutLineNum(image, line_up, line_down);
			
			for (int j = 0; gVerticalCutNum[j] != 0; j++)
			{
				//cout << gVerticalCutNum[j] << endl;
				if (j % 2 == 0)
				{
					//cout << "     " << gVerticalCutNum[j] << endl;
					words[wordsNum].up_line = line_up;
					words[wordsNum].down_line = line_down;
					words[wordsNum].left_line = gVerticalCutNum[j];
					words[wordsNum].right_line = gVerticalCutNum[j + 1];
					words[wordsNum].line_num = (line_up + 2) / 2;

					//以下对字符进行压缩
					if (wordsNum == 87)
					{
						//cout << words[wordsNum].up_line << " " << words[wordsNum].down_line << " " << words[wordsNum].left_line << " " << words[wordsNum].right_line << endl;
					}

					int tmp_horizon_up = 0;
					int tmp_horizon_down = 0;
					tmp_horizon_up = words[wordsNum].up_line;
					for (int i_up = words[wordsNum].up_line; i_up <= words[wordsNum].down_line; i_up++)
					{
						int flag_up = 0;
						uchar* data_up = image.ptr<uchar>(i_up);
						for (int j = words[wordsNum].left_line; j <= words[wordsNum].right_line; j++)
						{
							if (data_up[j] == 0)
								flag_up++;
						}
						if (flag_up == 0)
							tmp_horizon_up = i_up + 1;
						else
						{
							break;
						}
					}

					//cout << "tmp_horizon_up" << tmp_horizon_up << endl;
					tmp_horizon_down = words[wordsNum].down_line;
					for (int i_down = words[wordsNum].down_line; i_down >= words[wordsNum].up_line; i_down--)
					{
						int flag_down = 0;
						uchar* data_down = image.ptr<uchar>(i_down);
						for (int j = words[wordsNum].left_line; j <= words[wordsNum].right_line; j++)
						{
							if (data_down[j] == 0)
								flag_down++;
						}
						if (flag_down == 0)
							tmp_horizon_down = i_down - 1;
						else
							break;
					}

					//cout << "tmp_horizon_down" << tmp_horizon_down << endl;
					words[wordsNum].up_line = tmp_horizon_up;
					words[wordsNum].down_line = tmp_horizon_down;
					words[wordsNum].widthPixel = words[wordsNum].right_line - words[wordsNum].left_line;
					words[wordsNum].heightPixel = words[wordsNum].down_line - words[wordsNum].up_line;
					words[wordsNum].whScale = (double)words[wordsNum].widthPixel / (double)words[wordsNum].heightPixel;

					//cout << wordsNum << " " << endl;
					//if (wordsNum == 0);
					//else
					//	cout << words[wordsNum].left_line - words[wordsNum - 1].right_line << endl;

					
					wordsNum++;
				}
			}
		}
	}



	/*cout << "up_line" << "\t" << "down_line" << "\t" << "left_line" << "\t" << "right_line" << endl;
	for (int i = 0; i < wordsNum; i++)
	{
		cout << words[i].up_line << "\t" << words[i].down_line << "\t" << words[i].left_line << "\t" << words[i].right_line << endl;
	}

	std::cout << "initWord down!" << endl;*/
	return words;
}

/*wordImformation* compressionWords(Mat& image, wordImformation& word)//压缩单个字符的上下空间
{
	int tmp_horizon_up = 0;
	int tmp_horizon_down = 0;

	tmp_horizon_up = word.up_line;
	for (int i_up = word.up_line; i_up <= word.down_line; i_up++)
	{
		int flag_up = 0;
		uchar* data_up = image.ptr<uchar>(i_up);
		for (int j = word.left_line; j <= word.right_line; j++)
		{
			if (data_up[j] == 0)
				flag_up++;
		}
		if (flag_up == 0)
			tmp_horizon_up = i_up;
		else
		{
			break;
		}
	}

	cout << "tmp_horizon_up" << tmp_horizon_up << endl;
	tmp_horizon_down = word.down_line;
	for (int i_down = word.down_line; i_down >= word.up_line; i_down--)
	{
		int flag_down = 0;
		uchar* data_down = image.ptr<uchar>(i_down);
		for (int j = word.left_line; j <= word.right_line; j++)
		{
			if (data_down[j] == 0)
				flag_down++;
		}
		if (flag_down == 0)
			tmp_horizon_down = i_down;
		else
			break;
	}

	cout << "tmp_horizon_down" << tmp_horizon_down << endl;
	word.up_line = tmp_horizon_up;
	word.down_line = tmp_horizon_down;
	word.widthPixel = word.right_line - word.left_line;
	word.heightPixel = word.down_line - word.up_line;
	word.whScale = word.widthPixel / word.heightPixel;
	//std::cout << "compressionWords down" << endl;
	return &word;
}*/

wordImformation* getWordImformation(Mat& image, wordImformation words[])//注意要将连通之后的那张图传进来：label
{
	Mat label = unicom_area(image);
	//getPixelTxt(label);
	initWord(label, words);
	/*for (int i = 0; i < wordsNum; i++)
	{
		//std::cout << i << endl;
		//std::cout << "up_line:" << words[i].up_line << "\t" << "down_line:" << words[i].down_line << endl;
		//std::cout << "left_line:" << words[i].left_line << "\t" << "right_line" << words[i].right_line << endl;
		//std::cout << endl;



		for (int p = words[i].up_line; p <= words[i].down_line; p++)
		{
			uchar* data = image.ptr<uchar>(p);
			for (int q = words[i].left_line; q <= words[i].right_line; q++)
			{
				if (p == words[i].up_line || p == words[i].down_line)
				{
					data[q] = 0;
				}
				if (q == words[i].left_line || q == words[i].right_line)
					data[q] = 0;
			}
		}
	}*/
	//imshow("sds", image);
	//waitKey(0);
	//std::cout << "getWordImformation down!" << endl;
	return words;
}