#pragma once
#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stack>

using namespace cv;
using namespace std;

class CrackInfo //[CSDNԴ��]
{
public:
	CrackInfo(Point& position, long length, float width) {};
};
class CvGeFunctions //�������㼰ͼ��������Ҫ�ĺ���
{
public:
	static void addContrast(Mat& srcImg);// ���ӶԱȶ� [CSDNԴ��]
	static void swapMat(Mat& srcImg, Mat& dstImg);// ��������Mat [CSDNԴ��]
	static void binaryzation(Mat& srcImg); // ��ֵ��ͼ��0->0,��0->255 [CSDNԴ��]
	static void findConnectedDomain(Mat& srcImg, vector<vector<Point>>& connectedDomains, int area, int WHRatio);// �����ͨ�򣬲�ɾ����������������ͨ��[CSDNԴ��]
	static void thinImage(Mat& srcImg);// ��ȡ��ͨ��ĹǼ�[CSDNԴ��]
	static Point calInfoPosition(int imgRows, int imgCols, int padding, const std::vector<cv::Point>& domain); //��������Ϣ�ķ���λ��[CSDNԴ��]
	static void getWhitePoints(Mat& srcImg, vector<Point>& domain); // ��ȡͼ���а׵������[CSDNԴ��]
	static void turnBlackCrackToRed(Mat& redRes); //����ɫ�ѷ�תΪ��ɫ[07-09]
	static int getCannyThresholdUp(Mat& src, int tol = 2); //����Canny��Ե����������ֵ[07-09]
	static void setEdgeBlack(Mat& src); //����ԵͿ��[07-09]
	static double getChangeRatio(double Rcmos, double focal, double dis); //�������ض�Ӧ��ʵ�ʾ���ת��ϵ������Ϊ����[07-10] 
};