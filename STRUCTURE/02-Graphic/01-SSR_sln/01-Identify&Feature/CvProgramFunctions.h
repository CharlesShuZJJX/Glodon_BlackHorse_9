#pragma once
//#include "GlobalParams.h"
#include "CvGeFunctions.h"
#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <stack>
#include <map>

class CvProgramFunctions //������������ĸ��ຯ��
{
public:
	static ErrorStatus showCracks(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg); //����ѷ�ͼ��[07-09]
	//static ErrorStatus getConnectedDomain2Value(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg);//��ֵ��
	static ErrorStatus FillConnectDomain(Mat& img, vector<vector<Point>> vecDomain);
	static ErrorStatus getConnectedDomain(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg); //��ȡ��ͨ�� [07-09]
	static ErrorStatus getCrackLengthByRatio(vector<vector<Point>>& vecCracks, multimap<double, Point>& mapLength_Pos);//����ѷ쳤��[07-10]
	static ErrorStatus getCrackWidthByRatio(vector<vector<Point>>& vecCracks, multimap<double, Point>& mapWid_Pos);//����ѷ���[07-13]
	static ErrorStatus getCrackAreaByRatio(vector<vector<Point>>& vecCracks, multimap<double, Point>& mapArea_Pos);//����ѷ����[07-13]
private:
	bool comparePoint(Point pt1, Point pt2);//����Point���Զ�������ȽϺ�����map��
};