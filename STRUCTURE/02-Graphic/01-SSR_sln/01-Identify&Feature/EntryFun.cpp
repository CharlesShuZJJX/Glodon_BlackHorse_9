// TestOpenCv.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#pragma once
#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "CvGeFunctions.h"
#include <vector>
#include <stack>
#include "CvProgramFunctions.h"
#include "EXIF.h"
#include "GlobalParams.h"
using namespace std;
using namespace cv;
int main()
{
	int errorStatue = CvProgramFunctions::showCracks();
	if (errorStatue == ERROR_OPENFILE)
	{
		printf("打开图片失败");
	}
	return 0;
}
