#include "CvGeFunctions.h"
using namespace std;
using namespace cv;

void CvGeFunctions::addContrast(Mat& srcImg)
{
	Mat lookUpTable(1, 256, CV_8U);
	double temp = pow(1.1, 5);
	uchar* p = lookUpTable.data;
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(i * temp);
	LUT(srcImg, lookUpTable, srcImg);
}
void CvGeFunctions::swapMat(Mat& srcImg, Mat& dstImg)
{
	Mat tempImg = srcImg;
	srcImg = dstImg;
	dstImg = tempImg;
}
void CvGeFunctions::binaryzation(Mat& srcImg) 
{
	Mat lookUpTable(1, 256, CV_8U, Scalar(255));
	lookUpTable.data[0] = 0;
	LUT(srcImg, lookUpTable, srcImg);
}
/* 检测连通域，并删除不符合条件的连通域 */
void CvGeFunctions::findConnectedDomain(Mat& srcImg, vector<vector<Point>>& connectedDomains, int area, int WHRatio) 
{
	Mat_<uchar> tempImg = (Mat_<uchar>&)srcImg;

	for (int i = 0; i < tempImg.rows; ++i) 
	{
		uchar* row = tempImg.ptr(i);    ////调取存储图像内存的第i行的指针
		for (int j = 0; j < tempImg.cols; ++j) 
		{
			if (row[j] == 255) 
			{
				stack<Point> connectedPoints;
				vector<Point> domain;
				connectedPoints.push(Point(j, i));
				while (!connectedPoints.empty()) 
				{
					Point currentPoint = connectedPoints.top();
					domain.push_back(currentPoint);

					int colNum = currentPoint.x;
					int rowNum = currentPoint.y;

					tempImg.ptr(rowNum)[colNum] = 0;
					connectedPoints.pop();

					if (rowNum - 1 >= 0 && colNum - 1 >= 0 && tempImg.ptr(rowNum - 1)[colNum - 1] == 255) 
					{
						tempImg.ptr(rowNum - 1)[colNum - 1] = 0;
						connectedPoints.push(Point(colNum - 1, rowNum - 1));
					}
					if (rowNum - 1 >= 0 && tempImg.ptr(rowNum - 1)[colNum] == 255) 
					{
						tempImg.ptr(rowNum - 1)[colNum] = 0;
						connectedPoints.push(Point(colNum, rowNum - 1));
					}
					if (rowNum - 1 >= 0 && colNum + 1 < tempImg.cols && tempImg.ptr(rowNum - 1)[colNum + 1] == 255) 
					{
						tempImg.ptr(rowNum - 1)[colNum + 1] = 0;
						connectedPoints.push(Point(colNum + 1, rowNum - 1));
					}
					if (colNum - 1 >= 0 && tempImg.ptr(rowNum)[colNum - 1] == 255) 
					{
						tempImg.ptr(rowNum)[colNum - 1] = 0;
						connectedPoints.push(Point(colNum - 1, rowNum));
					}
					if (colNum + 1 < tempImg.cols && tempImg.ptr(rowNum)[colNum + 1] == 255) 
					{
						tempImg.ptr(rowNum)[colNum + 1] = 0;
						connectedPoints.push(Point(colNum + 1, rowNum));
					}
					if (rowNum + 1 < tempImg.rows && colNum - 1 > 0 && tempImg.ptr(rowNum + 1)[colNum - 1] == 255) 
					{
						tempImg.ptr(rowNum + 1)[colNum - 1] = 0;
						connectedPoints.push(Point(colNum - 1, rowNum + 1));
					}
					if (rowNum + 1 < tempImg.rows && tempImg.ptr(rowNum + 1)[colNum] == 255) 
					{
						tempImg.ptr(rowNum + 1)[colNum] = 0;
						connectedPoints.push(Point(colNum, rowNum + 1));
					}
					if (rowNum + 1 < tempImg.rows && colNum + 1 < tempImg.cols && tempImg.ptr(rowNum + 1)[colNum + 1] == 255) 
					{
						tempImg.ptr(rowNum + 1)[colNum + 1] = 0;
						connectedPoints.push(Point(colNum + 1, rowNum + 1));
					}
				}
				if (domain.size() > area) 
				{
					RotatedRect rect = minAreaRect(domain);
					float width = rect.size.width;
					float height = rect.size.height;
					if (width < height) 
					{
						float temp = width;
						width = height;
						height = temp;
					}
					if (width > height * WHRatio && width > 50) 
					{
						for (auto cit = domain.begin(); cit != domain.end(); ++cit) 
						{
							tempImg.ptr(cit->y)[cit->x] = 250;
						}
						connectedDomains.push_back(domain);
					}
				}
			}
		}
	}

	binaryzation(srcImg);
}
void CvGeFunctions::getSkeletonCurve(Mat& srcImg, vector<vector<Point>>& vecSkeletonCurves)
{
	vector<Point> deleteList;
	int neighbourhood[9];
	int nl = srcImg.rows;
	int nc = srcImg.cols;
	bool inOddIterations = true;
	while (true) {
		for (int j = 1; j < (nl - 1); j++) 
		{
			uchar* data_last = srcImg.ptr<uchar>(j - 1);
			uchar* data = srcImg.ptr<uchar>(j);
			uchar* data_next = srcImg.ptr<uchar>(j + 1);
			for (int i = 1; i < (nc - 1); i++) 
			{
				if (data[i] == 255) 
				{
					int whitePointCount = 0;
					neighbourhood[0] = 1;
					if (data_last[i] == 255) neighbourhood[1] = 1;
					else  neighbourhood[1] = 0;
					if (data_last[i + 1] == 255) neighbourhood[2] = 1;
					else  neighbourhood[2] = 0;
					if (data[i + 1] == 255) neighbourhood[3] = 1;
					else  neighbourhood[3] = 0;
					if (data_next[i + 1] == 255) neighbourhood[4] = 1;
					else  neighbourhood[4] = 0;
					if (data_next[i] == 255) neighbourhood[5] = 1;
					else  neighbourhood[5] = 0;
					if (data_next[i - 1] == 255) neighbourhood[6] = 1;
					else  neighbourhood[6] = 0;
					if (data[i - 1] == 255) neighbourhood[7] = 1;
					else  neighbourhood[7] = 0;
					if (data_last[i - 1] == 255) neighbourhood[8] = 1;
					else  neighbourhood[8] = 0;
					for (int k = 1; k < 9; k++) 
					{
						whitePointCount = whitePointCount + neighbourhood[k];
					}
					if ((whitePointCount >= 2) && (whitePointCount <= 6)) {
						int ap = 0;
						if ((neighbourhood[1] == 0) && (neighbourhood[2] == 1)) ap++;
						if ((neighbourhood[2] == 0) && (neighbourhood[3] == 1)) ap++;
						if ((neighbourhood[3] == 0) && (neighbourhood[4] == 1)) ap++;
						if ((neighbourhood[4] == 0) && (neighbourhood[5] == 1)) ap++;
						if ((neighbourhood[5] == 0) && (neighbourhood[6] == 1)) ap++;
						if ((neighbourhood[6] == 0) && (neighbourhood[7] == 1)) ap++;
						if ((neighbourhood[7] == 0) && (neighbourhood[8] == 1)) ap++;
						if ((neighbourhood[8] == 0) && (neighbourhood[1] == 1)) ap++;
						if (ap == 1) 
						{
							if (inOddIterations && (neighbourhood[3] * neighbourhood[5] * neighbourhood[7] == 0)
								&& (neighbourhood[1] * neighbourhood[3] * neighbourhood[5] == 0)) 
							{
								deleteList.push_back(Point(i, j));
							}
							else if (!inOddIterations && (neighbourhood[1] * neighbourhood[5] * neighbourhood[7] == 0)
								&& (neighbourhood[1] * neighbourhood[3] * neighbourhood[7] == 0)) 
							{
								deleteList.push_back(Point(i, j));
							}
						}
					}
				}
			}
		}
		if (deleteList.size() == 0)
			break;
		for (size_t i = 0; i < deleteList.size(); i++) {
			Point tem;
			tem = deleteList[i];
			uchar* data = srcImg.ptr<uchar>(tem.y);
			data[tem.x] = 0;
		}
		deleteList.clear();

		inOddIterations = !inOddIterations;
	}
	setEdgeBlack(srcImg);
	string strDir;
	strDir = ("skle%d.png");
	imwrite(GLOBAL_OUTPUT_DIR + strDir, srcImg);
	findConnectedDomain(srcImg, vecSkeletonCurves, 0, 0);
}

void CvGeFunctions::getWhitePoints(Mat& srcImg, vector<Point>& domain) 
{
	domain.clear();
	Mat_<uchar> tempImg = (Mat_<uchar>&)srcImg;
	for (int i = 0; i < tempImg.rows; i++) 
	{
		uchar* row = tempImg.ptr<uchar>(i);
		for (int j = 0; j < tempImg.cols; ++j) 
		{
			if (row[j] != 0)
				domain.push_back(Point(j, i));
		}
	}
}
/* 计算宽高信息的放置位置 */
Point CvGeFunctions::calInfoPosition(int imgRows, int imgCols, int padding, const std::vector<cv::Point>& domain)
{
	long xSum = 0;
	long ySum = 0;
	for (auto it = domain.cbegin(); it != domain.cend(); ++it) 
	{
		xSum += it->x;
		ySum += it->y;
	}
	int x = 0;
	int y = 0;
	x = (int)(xSum / domain.size());
	y = (int)(ySum / domain.size());
	if (x < padding)
		x = padding;
	if (x > imgCols - padding)
		x = imgCols - padding;
	if (y < padding)
		y = padding;
	if (y > imgRows - padding)
		y = imgRows - padding;

	return cv::Point(x, y);
}
void CvGeFunctions::turnBlackCrackToRed(Mat& redRes)
{
	int cPointR, cPointG, cPointB, cPoint;//currentPoint;
	for (int i = 1; i < redRes.rows; i++)
	{
		for (int j = 1; j < redRes.cols; j++)
		{
			cPointB = redRes.at<Vec3b>(i, j)[0];
			cPointG = redRes.at<Vec3b>(i, j)[1];
			cPointR = redRes.at<Vec3b>(i, j)[2];
			if (cPointB < 1 & cPointR < 1 & cPointG < 1)
			{
				redRes.at<Vec3b>(i, j)[0] = 0;
				redRes.at<Vec3b>(i, j)[1] = 0;
				redRes.at<Vec3b>(i, j)[2] = 255;
			}
		}
	}
}
int CvGeFunctions::getCannyThresholdUp(Mat& src, int tol)
{
	int nRow = src.rows;
	int nCol = src.cols;
	int grayMax = INT_MIN;
	int grayMin = INT_MAX;
	int grayAvg = 0;
	int doStep = 0;
	while (doStep <= 100)
	{
		if (doStep == 0)
		{
			for (int i = 0; i < nRow; ++i)
			{
				uchar* data = src.ptr<uchar>(i);
				for (int j = 0; j < nCol; ++j)
				{
					grayMax = grayMax > data[j] ? grayMax : data[j];
					grayMin = grayMin < data[j] ? grayMin : data[j];
				}
				grayAvg = (grayMax + grayMin) / 2;
			}
		}

		long darkAvg = 0;
		int numDark = 0;
		long lightAvg = 0;
		int numLight = 0;
		for (int i = 0; i < nRow; ++i)
		{
			uchar* data = src.ptr<uchar>(i);
			for (int j = 0; j < nCol; ++j)
			{
				if (data[j] >= grayAvg)//light
				{
					lightAvg += data[j];
					++numLight;
				}
				else
				{
					darkAvg += data[j];
					++numDark;
				}
			}
		}
		if (numLight == 0)
		{
			lightAvg = 0;
		}
		else
		{
			lightAvg /= numLight;
		}
		if (numDark == 0)
		{
			darkAvg = 0;
		}
		else
		{
			darkAvg /= numDark;
		}
		if (abs((lightAvg + darkAvg) / 2 - grayAvg) <= tol)
		{
			printf("已完成自适应阈值划分,灰度均值为%d \n", grayAvg);
			return (lightAvg + darkAvg) / 2;
		}
		else
		{
			grayAvg = (lightAvg + darkAvg) / 2;
		}
		++doStep;
	}
	return grayAvg;
}
void CvGeFunctions::setEdgeBlack(Mat& src)
{
	int nRow = src.rows;
	int nCol = src.cols;
	for (int i = 0; i < nRow; ++i)
	{
		uchar* data = src.ptr<uchar>(i);
		if (i == 0 || i == nRow - 1)
		{
			for (int j = 0; j < nCol; ++j)
			{
				data[j] = 0;
			}
		}
		else
		{
			data[0] = 0;
			data[nCol - 1] = 0;
		}
	}
}
double CvGeFunctions::getChangeRatio(double Rcmos, double focal, double dis)
{
	return Rcmos * (dis * dis + dis * sqrt(dis * dis / 4.0 - focal * dis) - focal * dis) / (focal * dis);
}
double CvGeFunctions::getChangeRatio(double refRealLen, double refPixelLen)
{
	return refRealLen / refPixelLen;
}
void  CvGeFunctions::fillPtsToBlack(Mat& src, vector<Point> ptsToFill)
{
	int nRow = src.rows;
	int nCol = src.cols;
	for (int i = 0; i < ptsToFill.size(); ++i)
	{
		int x = ptsToFill[i].y;
		int y = ptsToFill[i].x;
		if (x < nRow && y < nCol)
		{
			uchar* data = src.ptr<uchar>(x);
			data[y] = 0;
		}
	}
}