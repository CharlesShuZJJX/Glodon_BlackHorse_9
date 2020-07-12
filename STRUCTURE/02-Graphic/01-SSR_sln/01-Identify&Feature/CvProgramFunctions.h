#pragma once
#include "GlobalParams.h"
#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "CvGeFunctions.h"
#include <vector>
#include <stack>
#include <map>

class CvProgramFunctions //程序运行所需的各类函数
{
public:
	static ErrorStatus showCracks(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg); //输出裂缝图像[07-09]
	static ErrorStatus getConnectedDomain(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg); //获取连通域 [07-09]
	static ErrorStatus getCrackLengthByRatio(vector<vector<Point>>& vecCracks, multimap<double, Point>& mapLength_Pos, double changeRatio);//输出裂缝长度[07-10]
private:
	bool comparePoint(Point pt1, Point pt2);//两个Point的自定义排序比较函数，map用
};