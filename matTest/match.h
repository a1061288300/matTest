#include <iostream>
#include <opencv2/opencv.hpp>
#include <highgui.hpp>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <io.h>
#include <stdlib.h>
#include <map>
using namespace std;
using namespace cv;


map<char, double> matchImform;
map<char, double> WtoHArr;
const int total = 2500;


void getModeNorNameArr(char* modePreName, string modeNorNameArr[], string modeImformNameArr[])//��ȡXXnor.txt �� XXimform.txt
{
	_finddata_t modeDir;
	string preName = (string)modePreName;
	preName = preName.substr(0, preName.length() - 3);
	long lfDir;
	int nortmp = 0, imformtmp = 0;
	if ((lfDir = _findfirst(modePreName, &modeDir)) == -1l)
	{
		cout << "�Ҳ���ģ���ļ�" << endl;
	}
	else
	{
		do{
			string modeName = (string)modeDir.name;
			
			if (modeName.length() >= 7 && (modeName.substr(modeName.length() - 7, 3) == "nor"))
			{
				modeNorNameArr[nortmp] = preName + modeName;
				nortmp++;
			}
			else if (modeName.length() >= 10 && (modeName.substr(modeName.length() - 10, 6) == "imform"))
			{
				modeImformNameArr[imformtmp] = preName + modeName;
				imformtmp++;
			}

		} while (_findnext(lfDir, &modeDir) == 0);
	}
	_findclose(lfDir);

}

void getMatchNameArr(char* matchPreName, string matchNameArr[])
{
	for (int i = 0; i < wordsNum; i++)
	{
		string matchTmpName = matchPreName;
		int tt = i;
		string ss;
		if (tt == 0)
			matchTmpName += '0';
		while (tt != 0)
		{
			int tmp = tt % 10;
			ss += (tmp + '0');
			tt /= 10;
		}

		reverse(ss.begin(), ss.end());
		matchTmpName += ss;
		matchTmpName += ".txt";
		matchNameArr[i] = matchTmpName;
	}
}

int getNorNumber(string ss)//��ȡ��XXnor.txt�ļ���norǰ�������
{
	string tmps = ss.substr(0, ss.length() - 7);
	int len = tmps.length() - 1;
	while (tmps[len] - '0' >= 0 && tmps[len] - '0' <= 9)
	{
		len--;
	}
	int num = 0;
	len++;
	while (tmps[len] - '0' >= 0 && tmps[len] - '0' <= 9)
	{
		num *= 10;
		num += (tmps[len] - '0');
		len++;
	}
	
	return num;
}

int getImformNumber(string ss)//��ȡ��XXimform.txt�ļ���imformǰ�������
{
	string tmps = ss.substr(0, ss.length() - 10);
	int len = tmps.length() - 1;
	while (tmps[len] - '0' >= 0 && tmps[len] - '0' <= 9)
	{
		len--;
	}
	int num = 0;
	len++;
	while (tmps[len] - '0' >= 0 && tmps[len] - '0' <= 9)
	{
		num *= 10;
		num += (tmps[len] - '0');
		len++;
	}
	return num;
}

void getWtoHArr(wordImformation word, string matchName, string modeName)//�õ���߱�  ƥ��Ľ��  bematch.whscale / mode.whscale
{

	double whscale = word.whScale;

	ifstream modeIn(modeName);
	char tmp[10];
	if (!modeIn.is_open())
	{
		cout << "�򿪿�߱��ļ�ʧ��" << endl;
		exit(1);
	}
	modeIn.getline(tmp, 10);
	double modeWHscale = atof(tmp);
	//cout << "modeWHscale: " << modeWHscale << endl;
	//cout << "whscale: " << whscale << endl; 
	int k = getImformNumber(modeName);
	if (modeWHscale == 0)
		WtoHArr[char(k)] = 0;
	else
	{
		WtoHArr[char(k)] = whscale / modeWHscale;
	}

}


void getMatchArr(string matchName, string modeName)//�õ�����ƥ�����ƶ�
{

	char bematch[100];
	char mode[100];
	int len = strlen(bematch);

	ifstream matchIn(matchName);
	ifstream modeIn(modeName);

	if (!matchIn.is_open())
	{
		cout << "�򿪴�ƥ���ļ�ʧ��" << endl;
		exit(1);
	}
	if (!modeIn.is_open())
	{
		cout << "��ģ���ļ�ʧ��" << endl;
		exit(1);
	}


	int count = 0;

	for (int i = 0; i < 50; i++)
	{
		matchIn.getline(bematch, 100);
		modeIn.getline(mode, 100);
		int len = strlen(bematch);
		for (int j = 0; j < len; j++)
		{
			if (bematch[j] == mode[j])
			{
				count++;
			}
		}
	}
	//cout << "count: " << count << endl;
	//cout << "total: " << total << endl;
	double res = (double)count / (double)total;
	//cout << "res: " << res << endl;
	int k = getNorNumber(modeName);
	matchImform[char(k)] = res;
	matchIn.close();
	modeIn.close();
	
}