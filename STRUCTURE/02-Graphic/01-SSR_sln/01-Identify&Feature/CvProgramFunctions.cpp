#include"CvProgramFunctions.h"

ErrorStatus CvProgramFunctions::getConnectedDomain(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg)
{
	Mat srcOriginPic = orgImg;
	Mat srcOriginPicBGR = srcOriginPic;
	if (srcOriginPic.empty())
	{
		return ERROR_OPENFILE;
	}
	cvtColor(srcOriginPic, resImg, COLOR_BGR2GRAY, 1); // 改为灰度图 
	CvGeFunctions::addContrast(srcOriginPic); // 加强对比度 
	CvGeFunctions::swapMat(srcOriginPic, resImg);

	int avgGray = CvGeFunctions::getCannyThresholdUp(srcOriginPic);

	int cannyMax = (avgGray) > 255 ? 255 : (avgGray);
	int cannyMin = cannyMax / 5; // 上下阈值之比为5:1 [ssr 07-09]
	Canny(srcOriginPic, resImg, cannyMin, cannyMax, 3); // 边缘检测 
	vector<Point> outPts; //记录白色轮廓
	CvGeFunctions::getWhitePoints(resImg, outPts);
	GLOBAL_OUTPTS = outPts;
	imwrite(GLOBAL_OUTPUT_DIR + "canneyRes.png", resImg);

	//形态学变换,不能膨胀，因为会导致连通域变宽，宽度失真
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(resImg, resImg, MORPH_CLOSE, kernel, Point(-1, -1), 3);
	kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	erode(resImg, resImg, kernel);

	CvGeFunctions::setEdgeBlack(resImg);
	//CvGeFunctions::fillPtsToBlack(resImg, outPts);

	imwrite(GLOBAL_OUTPUT_DIR + "afterKernelRes.png", resImg);

	//寻找连通域
	CvGeFunctions::findConnectedDomain(resImg, connectedDomains, 2, 0); //area和WHRatio调整为0，增加识别几率
	imwrite(GLOBAL_OUTPUT_DIR + "orgConnRes.png", srcOriginPicBGR);
	return SUCCEED;
}
ErrorStatus CvProgramFunctions::showCracks(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg)
{
	cout << "开始测量" << endl;
	cout << "连通域数量：" << connectedDomains.size() << endl;
	Mat lookUpTable(1, 256, CV_8U, Scalar(0));
	vector<CrackInfo> crackInfos;
	vector<vector<Point>> vecSkeletonCurves;
	imwrite(GLOBAL_OUTPUT_DIR + "resultn.png", resImg);
	for (auto domain_it = connectedDomains.begin(); domain_it != connectedDomains.end(); ++domain_it)
	{
		LUT(resImg, lookUpTable, resImg);
		for (auto point_it = domain_it->cbegin(); point_it != domain_it->cend(); ++point_it)
		{
			resImg.ptr<uchar>(point_it->y)[point_it->x] = 255;
		}
		double area = (double)domain_it->size();
		
		CvGeFunctions::getSkeletonCurve(resImg, vecSkeletonCurves);
		GLOBAL_VEC_CRACK.push_back(crackInterface(*domain_it, vecSkeletonCurves[vecSkeletonCurves.size() - 1]));
	}
	cout << "开始绘制信息" << endl;
	cout << "信息数量：" << vecSkeletonCurves.size() << endl;

	LUT(resImg, lookUpTable, resImg);

	int step = 1;
	for (auto domain_it = vecSkeletonCurves.cbegin(); domain_it != vecSkeletonCurves.cend(); ++domain_it)
	{
		if (step == 100)
		{
			++step;
			continue;
		}
		for (auto point_it = domain_it->cbegin(); point_it != domain_it->cend(); ++point_it)
		{
			resImg.ptr<uchar>(point_it->y)[point_it->x] = 255;
		}
		++step;
	}

	resImg = ~resImg; //颜色取反，获取白底黑缝的图案[ssr 07-09]
	imwrite(GLOBAL_OUTPUT_DIR + "result1.png", resImg);
	Mat redRes = imread(GLOBAL_OUTPUT_DIR + "result1.png");

	CvGeFunctions::turnBlackCrackToRed(redRes);//裂缝颜色取红色[ssr 07-09]
	imwrite(GLOBAL_OUTPUT_DIR + "redRes.png", redRes);

	Mat mix;
	addWeighted(redRes, 0.5, orgImg, 0.5, 0.0, mix);//裂缝图和原图按1:1权重混合 [ssr 07-09]
	imwrite(GLOBAL_OUTPUT_DIR + "mixRes.png", mix);

	cout << "保存图像完成" << endl;
	multimap<double, Point> mapLength_Pos, mapWid_Pos, mapArea_Pos;
	getCrackLengthByRatio(vecSkeletonCurves, mapLength_Pos);
	getCrackWidthByRatio(vecSkeletonCurves, mapWid_Pos);
	getCrackAreaByRatio(vecSkeletonCurves, mapArea_Pos);
	return SUCCEED;
}
bool CvProgramFunctions::comparePoint(Point pt1, Point pt2)
{
	if (pt1.x < pt2.x)
	{
		return true;
	}
	else if (pt1.x > pt2.x)
	{
		return false;
	}
	else if (pt1.y < pt2.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}
ErrorStatus CvProgramFunctions::getCrackLengthByRatio()
{
	for (int i = 0; i < GLOBAL_VEC_CRACK.size(); ++i)
	{
		if (GLOBAL_VEC_CRACK[i].m_vecSkl.size() == 0)
		{
			continue;
		}
		int pixelLength = GLOBAL_VEC_CRACK[i].m_vecSkl.size();
		Point crPos = GLOBAL_VEC_CRACK[i].m_vecSkl[pixelLength / 2];
		double trueLength = pixelLength * GLOBAL_PIXEL_TO_REAL_RATIO;
		//mapLength_Pos.insert(std::make_pair( trueLength, crPos));
		printf("裂缝位置：(%d, %d)，长度:%f像素\n", crPos.y, crPos.x, trueLength);
	}
	return SUCCEED;
}
ErrorStatus CvProgramFunctions::getCrackWidthByRatio()
{
	for (int i = 0; i < GLOBAL_VEC_CRACK.size(); ++i)
	{
		if (GLOBAL_VEC_CRACK[i].m_vecSkl.size() == 0)
		{
			continue;
		}
		double crackDis = DBL_MIN;
		int pixelLength = GLOBAL_VEC_CRACK[i].m_vecSkl.size();
		Point crPos = vecCracks[i][pixelLength / 2];
		for (int j = 0; j < GLOBAL_VEC_CRACK[i].m_vecSkl.size(); ++j)
		{
			int xSkl = GLOBAL_VEC_CRACK[i].m_vecSkl[j].y;
			int ySkl = GLOBAL_VEC_CRACK[i].m_vecSkl[j].x;
			double minDis = DBL_MAX;
			for (int k = 0; k < GLOBAL_OUTPTS.size(); ++k)
			{
				
				int xProfile = GLOBAL_OUTPTS[k].y;
				int yProfile = GLOBAL_OUTPTS[k].x;
				double disThis = 2 * sqrt(pow(xSkl - xProfile - 1 / 2, 2) + pow(ySkl - yProfile - 1 / 2, 2));
				if (minDis > disThis)
				{
					minDis = disThis;
				}
			}
			if (crackDis < minDis)
			{
				crackDis = minDis;
			}
		}
		double trueWid = crackDis * GLOBAL_PIXEL_TO_REAL_RATIO;
		mapWid_Pos.insert(std::make_pair(trueWid, crPos));
		printf("裂缝位置：(%d, %d)，宽度:%f像素\n", crPos.y, crPos.x, trueWid);
	}
	return SUCCEED;
}
ErrorStatus CvProgramFunctions::getCrackAreaByRatio()
{
	for (int i = 0; i < GLOBAL_VEC_CRACK.size(); ++i)
	{
		if (GLOBAL_VEC_CRACK[i].m_vecDomain.size() == 0)
		{
			continue;
		}
		int pixelArea = GLOBAL_VEC_CRACK[i].m_vecDomain.size();
		int pixelLength = GLOBAL_VEC_CRACK[i].m_vecSkl.size();
		Point crPos = GLOBAL_VEC_CRACK[i].m_vecSkl[pixelLength / 2];
		double trueLength = pixelArea * GLOBAL_PIXEL_TO_REAL_RATIO;
		printf("裂缝位置：(%d, %d)，面积:%f像素\n", crPos.y, crPos.x, trueLength);
	}
	return SUCCEED;
}