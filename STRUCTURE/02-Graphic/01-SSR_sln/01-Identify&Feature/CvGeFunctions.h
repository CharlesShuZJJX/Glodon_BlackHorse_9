#pragma once
#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stack>
#include "GlobalParams.h"
#include "EXIF.h"
using namespace cv;
using namespace std;

class CrackInfo //[CSDN源码]
{
public:
	CrackInfo(Point& position, long length, float width) {};
};
class CvGeFunctions //参数计算及图像处理所需要的函数
{
public:
	static void addContrast(Mat& srcImg);// 增加对比度 [CSDN源码]
	static void swapMat(Mat& srcImg, Mat& dstImg);// 交换两个Mat [CSDN源码]
	static void binaryzation(Mat& srcImg); // 二值化图像。0->0,非0->255 [CSDN源码]
	static void findConnectedDomain(Mat& srcImg, vector<vector<Point>>& connectedDomains, int area, int WHRatio);// 检测连通域，并删除不符合条件的连通域[CSDN源码]
	static void getSkeletonCurve(Mat& srcImg, vector<vector<Point>>& vecSkeletonCurves);// 提取连通域的骨架[CSDN源码 / sr-0711]
	static Point calInfoPosition(int imgRows, int imgCols, int padding, const std::vector<cv::Point>& domain); //计算宽高信息的放置位置[CSDN源码]
	static void getWhitePoints(Mat& srcImg, vector<Point>& domain); // 获取图像中白点的数量[CSDN源码]
	static void turnBlackCrackToRed(Mat& redRes); //将黑色裂缝转为白色[07-09]
	static int getCannyThresholdUp(Mat& src, int tol = 2); //计算Canny边缘检测的上限阈值[07-09]
	static void setEdgeBlack(Mat& src); //将边缘涂黑[07-09]
	static double getChangeRatio(double Rcmos, double focal, double dis); //计算像素对应的实际距离转换系数，均为毫米，举例法[07-10] 
	static double getChangeRatio(double refRealLen, double refPixelLen); //计算像素对应的实际距离转换系数，均为毫米,参照法[07-13] 
	static void fillPtsToBlack(Mat& src, vector<Point> ptsToFill); //将指定点涂黑
};