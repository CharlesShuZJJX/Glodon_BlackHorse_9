#include "com_demo_opencv_NativeActivity.h"
#include<android/bitmap.h>
#include <opencv2/opencv.hpp>
#include <jni.h>
#include <string>
#include <string>
#include <math.h>
#include <iostream>
#include <stack>

using namespace std;
using namespace cv;

typedef int ErrorStatus;
enum { SUCCEED, FAILED, ERROR_OPENFILE, ERROR_WRITEFILE };

const string GLOBAL_OUTPUT_DIR;
const string GLOBAL_ORIGIN_JPG_DIR;

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
    static void  fillPtsToRed(Mat& src, vector<Point> ptsToFill);
};

class CvProgramFunctions //程序运行所需的各类函数
{
public:
    static ErrorStatus getConnectedDomain(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg); //获取连通域 [07-09]
    static ErrorStatus showCracks(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg); //输出裂缝图像[07-09]
};

class Cmd {
public:
    static void getEdge(); //只需执行这个命令
};

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
ErrorStatus CvGeFunctions::findConnectedDomain(Mat& srcImg, vector<vector<Point>>& connectedDomains, int area, int WHRatio)
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
    return SUCCEED;
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
    double grayAvg = 0.0;
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
                grayAvg = (double (grayMax + grayMin)) / 2;
            }
        }

        double darkAvg = 0.0;
        long numDark = 0;
        double lightAvg = 0.0;
        long numLight = 0;
        for (int i = 0; i < nRow; ++i)
        {
            uchar* data = src.ptr<uchar>(i);
            for (int j = 0; j < nCol; ++j)
            {
                if (data[j] >= grayAvg)//light
                {
                    ++numLight;
                }
                else
                {
                    ++numDark;
                }
            }
        }
        for (int i = 0; i < nRow; ++i)
        {
            uchar* data = src.ptr<uchar>(i);
            for (int j = 0; j < nCol; ++j)
            {
                if (data[j] >= grayAvg)//light
                {
                    lightAvg += (double(data[j])) / numLight;
                }
                else
                {
                    darkAvg += (double(data[j])) / numDark;
                }
            }
        }

        if (abs((lightAvg + darkAvg) / 2 - grayAvg) <= tol)
        {
            printf("已完成自适应阈值划分,灰度均值为%d \n", (int)grayAvg);
            return (int(lightAvg) + (darkAvg)) / 2;
        }
        else
        {
            grayAvg = (lightAvg + darkAvg) / 2;
        }
        ++doStep;
    }
    return grayAvg;
}
ErrorStatus CvGeFunctions::setEdgeBlack(Mat& src)
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
    return SUCCEED;
}
void  CvGeFunctions::fillPtsToRed(Mat& src, vector<Point> ptsToFill)
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
            data[y * 3] = 0;
            data[y * 3 + 1] = 0;
            data[y * 3 + 2] = 255;
        }
    }
}

/*  CvProgramFunctions                */
ErrorStatus CvProgramFunctions::getConnectedDomain(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg)
{
    if (orgImg.empty())
    {
        return ERROR_OPENFILE;
    }
    cvtColor(orgImg, resImg, COLOR_BGR2GRAY, 1); // res灰度图
    CvGeFunctions::addContrast(orgImg); // 加强对比度
    CvGeFunctions::addContrast(resImg); // 加强对比度

    int avgGray = CvGeFunctions::getCannyThresholdUp(resImg);
    int cannyMax = (avgGray) > 255 ? 255 : (avgGray);
    int cannyMin = cannyMax / 5; // 上下阈值之比为5:1 [ssr 07-09]
    Canny(orgImg, resImg, 50, 150, 3); // 边缘检测,覆盖之前的resImg

    //形态学变换,不能膨胀，因为会导致连通域变宽，宽度失真
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(resImg, resImg, MORPH_CLOSE, kernel, Point(-1, -1), 3);
    //kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    //erode(resImg, resImg, kernel);
    medianBlur(resImg, resImg, 3);
//    imwrite(GLOBAL_OUTPUT_DIR + ".jpg", resImg);
    int ret = CvGeFunctions::setEdgeBlack(resImg);
    //寻找连通域
    int ret_1 = CvGeFunctions::findConnectedDomain(resImg, connectedDomains, 0, 0); //area和WHRatio调整为0，增加识别几率

    return SUCCEED;
}
ErrorStatus CvProgramFunctions::showCracks(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg)
{
//    cout << "开始测量" << endl;
//    cout << "连通域数量：" << connectedDomains.size() << endl;
    if (orgImg.empty())
    {
        return ERROR_OPENFILE;
    }
    Mat lookUpTable(1, 256, CV_8U, Scalar(0));
    vector<CrackInfo> crackInfos;
    vector<vector<Point>> vecSkeletonCurves;
    for (auto domain_it = connectedDomains.begin(); domain_it != connectedDomains.end(); ++domain_it)
    {
        LUT(resImg, lookUpTable, resImg);
        for (auto point_it = domain_it->cbegin(); point_it != domain_it->cend(); ++point_it)
        {
            resImg.ptr<uchar>(point_it->y)[point_it->x] = 255;
        }
        double area = (double)domain_it->size();

        CvGeFunctions::getSkeletonCurve(resImg, vecSkeletonCurves);
    }
//    cout << "开始绘制" << endl;

    for (int i = 0; i < vecSkeletonCurves.size(); ++i)
    {
        CvGeFunctions::fillPtsToRed(orgImg, vecSkeletonCurves[i]);
    }
//    imwrite(GLOBAL_OUTPUT_DIR, orgImg);
//
//    cout << "保存图像完成" << endl;
    return SUCCEED;
}

/*  CvProgramFunctions                */
void Cmd::getEdge()
{
    Mat orgImg = imread(GLOBAL_ORIGIN_JPG_DIR);
    Mat resImg;
    vector<vector<Point>> connectedDomains;
    int errorStatue = CvProgramFunctions::getConnectedDomain(connectedDomains, orgImg, resImg);
    if (errorStatue == ERROR_OPENFILE)
    {
        printf("打开图片失败！");
        return;
    }
    CvProgramFunctions::showCracks(connectedDomains, orgImg, resImg);
}

bool BitmapToMatrix(JNIEnv * env, jobject obj_bitmap, cv::Mat & matrix) {
    void * bitmapPixels;                                            // 保存图片像素数据
    AndroidBitmapInfo bitmapInfo;                                   // 保存图片参数

    CV_Assert( AndroidBitmap_getInfo(env, obj_bitmap, &bitmapInfo) >= 0);        // 获取图片参数
    CV_Assert( bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888
                  || bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565 );          // 只支持 ARGB_8888 和 RGB_565
    CV_Assert( AndroidBitmap_lockPixels(env, obj_bitmap, &bitmapPixels) >= 0 );  // 获取图片像素（锁定内存块）
    CV_Assert( bitmapPixels );

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, bitmapPixels);    // 建立临时 mat
        tmp.copyTo(matrix);                                                         // 拷贝到目标 matrix
    } else {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, bitmapPixels);
        cv::cvtColor(tmp, matrix, cv::COLOR_BGR5652RGB);
    }

    AndroidBitmap_unlockPixels(env, obj_bitmap);            // 解锁
    return true;
}

bool MatrixToBitmap(JNIEnv * env, cv::Mat & matrix, jobject obj_bitmap) {
    void * bitmapPixels;                                            // 保存图片像素数据
    AndroidBitmapInfo bitmapInfo;                                   // 保存图片参数

    CV_Assert( AndroidBitmap_getInfo(env, obj_bitmap, &bitmapInfo) >= 0);        // 获取图片参数
    CV_Assert( bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888
                  || bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565 );          // 只支持 ARGB_8888 和 RGB_565
    CV_Assert( matrix.dims == 2
                  && bitmapInfo.height == (uint32_t)matrix.rows
                  && bitmapInfo.width == (uint32_t)matrix.cols );                   // 必须是 2 维矩阵，长宽一致
    CV_Assert( matrix.type() == CV_8UC1 || matrix.type() == CV_8UC3 || matrix.type() == CV_8UC4 );
    CV_Assert( AndroidBitmap_lockPixels(env, obj_bitmap, &bitmapPixels) >= 0 );  // 获取图片像素（锁定内存块）
    CV_Assert( bitmapPixels );

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, bitmapPixels);
        switch (matrix.type()) {
            case CV_8UC1:   cv::cvtColor(matrix, tmp, cv::COLOR_GRAY2RGBA);     break;
            case CV_8UC3:   cv::cvtColor(matrix, tmp, cv::COLOR_RGB2RGBA);      break;
            case CV_8UC4:   matrix.copyTo(tmp);                                 break;
            default:        AndroidBitmap_unlockPixels(env, obj_bitmap);        return false;
        }
    } else {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, bitmapPixels);
        switch (matrix.type()) {
            case CV_8UC1:   cv::cvtColor(matrix, tmp, cv::COLOR_GRAY2BGR565);   break;
            case CV_8UC3:   cv::cvtColor(matrix, tmp, cv::COLOR_RGB2BGR565);    break;
            case CV_8UC4:   cv::cvtColor(matrix, tmp, cv::COLOR_RGBA2BGR565);   break;
            default:        AndroidBitmap_unlockPixels(env, obj_bitmap);        return false;
        }
    }
    AndroidBitmap_unlockPixels(env, obj_bitmap);                // 解锁
    return true;
}



extern "C" JNIEXPORT void
JNICALL Java_com_demo_opencv_NativeActivity_getEdge
        (JNIEnv *env, jobject /* this */, jobject bitmap) {
    AndroidBitmapInfo info={0};
    void *pixels;

    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
//    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
//    CV_Assert(pixels);
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
//        cv::Mat& orgImg = *(cv::Mat*)bitmap;
//        Mat orgImg(info.height, info.width, CV_8UC4, pixels);
        Mat orgImg;
        bool ret = BitmapToMatrix(env, bitmap, orgImg);
        if (ret == false) {
            return;
        }
        Mat resImg;
        vector<vector<Point>> connectedDomains;
        int errorStatue = CvProgramFunctions::getConnectedDomain(connectedDomains, orgImg, resImg);
        int errorStatue_1 = CvProgramFunctions::showCracks(connectedDomains, orgImg, resImg);
//        CvGeFunctions::swapMat(resImg, orgImg);
        ret = MatrixToBitmap(env, orgImg, bitmap);
    }
    else{
        Mat orgImg(info.height, info.width, CV_8UC2, pixels);
        Mat resImg;
        vector<vector<Point>> connectedDomains;
//        int errorStatue = CvProgramFunctions::getConnectedDomain(connectedDomains, orgImg, resImg);
//        CvProgramFunctions::showCracks(connectedDomains, orgImg, resImg);
    }
//    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
//        Mat temp(info.height, info.width, CV_8UC4, pixels);
//        Mat gray;
//        cvtColor(temp, gray, COLOR_RGBA2GRAY);
//        Canny(gray, gray, 45, 75);
//        cvtColor(gray, temp, COLOR_GRAY2RGBA);
//    } else {
//        Mat temp(info.height, info.width, CV_8UC2, pixels);
//        Mat gray;
//        cvtColor(temp, gray, COLOR_RGB2GRAY);
//        Canny(gray, gray, 45, 75);
//        cvtColor(gray, temp, COLOR_GRAY2RGB);
//    }
//    AndroidBitmap_unlockPixels(env, bitmap);
}

































//#include <jni.h>
//#include <string>
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_demo_opencv_MainActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}
