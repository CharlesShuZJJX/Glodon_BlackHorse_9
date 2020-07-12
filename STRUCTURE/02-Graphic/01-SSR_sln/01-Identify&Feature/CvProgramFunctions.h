#pragma once
#include "GlobalParams.h"
#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "CvGeFunctions.h"
#include <vector>
#include <stack>
#include <map>

class CvProgramFunctions //������������ĸ��ຯ��
{
public:
	static ErrorStatus showCracks(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg); //����ѷ�ͼ��[07-09]
	static ErrorStatus getConnectedDomain(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg); //��ȡ��ͨ�� [07-09]
	static ErrorStatus getCrackLengthByRatio(vector<vector<Point>>& vecCracks, multimap<double, Point>& mapLength_Pos, double changeRatio);//����ѷ쳤��[07-10]
private:
	bool comparePoint(Point pt1, Point pt2);//����Point���Զ�������ȽϺ�����map��
};