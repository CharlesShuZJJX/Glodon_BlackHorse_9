#include"CvProgramFunctions.h"

ErrorStatus CvProgramFunctions::getConnectedDomain(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg)
{
	Mat srcOriginPic = orgImg;
	Mat srcOriginPicBGR = srcOriginPic;
	if (srcOriginPic.empty())
	{
		return ERROR_OPENFILE;
	}
	cvtColor(srcOriginPic, resImg, COLOR_BGR2GRAY, 1); // ��Ϊ�Ҷ�ͼ 
	CvGeFunctions::addContrast(srcOriginPic); // ��ǿ�Աȶ� 
	CvGeFunctions::swapMat(srcOriginPic, resImg);

	int avgGray = CvGeFunctions::getCannyThresholdUp(srcOriginPic);

	int cannyMax = (avgGray) > 255 ? 255 : (avgGray);
	int cannyMin = cannyMax / 5; // ������ֵ֮��Ϊ5:1 [ssr 07-09]
	Canny(srcOriginPic, resImg, cannyMin, cannyMax, 3); // ��Ե��� 
	imwrite(GLOBAL_OUTPUT_DIR + "canneyRes.png", resImg);

	//��̬ѧ�任
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	//dilate(resImg, resImg, kernel);//����
	//morphologyEx(resImg, resImg, MORPH_CLOSE, kernel, Point(-1, -1), 3);
	//morphologyEx(resImg, resImg, MORPH_CLOSE, kernel);

	CvGeFunctions::setEdgeBlack(resImg);

	imwrite(GLOBAL_OUTPUT_DIR + "afterKernelRes.png", resImg);

	//Ѱ����ͨ��
	CvGeFunctions::findConnectedDomain(resImg, connectedDomains, 0, 0); //area��WHRatio����Ϊ0������ʶ����
	kernel = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
	morphologyEx(resImg, resImg, MORPH_CLOSE, kernel, Point(-1, -1), 5);

/*
	for (int m = 0; m < connectedDomains.size(); ++m)
	{
		for (int n = 0; n < connectedDomains[m].size(); ++n)
		{
			int cPointR, cPointG, cPointB, cPoint;//currentPoint;
			int i = connectedDomains[m][n].y;
			int j = connectedDomains[m][n].x;

			srcOriginPicBGR.at<Vec3b>(i, j)[0] = 0;
			srcOriginPicBGR.at<Vec3b>(i, j)[1] = 0;
			srcOriginPicBGR.at<Vec3b>(i, j)[2] = 255;
		}
	}*/
	imwrite(GLOBAL_OUTPUT_DIR + "orgConnRes.png", srcOriginPicBGR);
	//connectedDomains.clear();
//	CvGeFunctions::findConnectedDomain(resImg, connectedDomains, 0, 0);
//	kernel = getStructuringElement(MORPH_CROSS, Size(3, 3));
//	morphologyEx(resImg, resImg, MORPH_CLOSE, kernel);

//	kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
//	erode(resImg, resImg, kernel);

//	connectedDomains.clear();
	CvGeFunctions::findConnectedDomain(resImg, connectedDomains, 0, 0);
	return SUCCEED;
}
ErrorStatus CvProgramFunctions::showCracks(vector<vector<Point>>& connectedDomains, Mat& orgImg, Mat& resImg)
{
	cout << "��ʼ����" << endl;
	cout << "��ͨ��������" << connectedDomains.size() << endl;
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
	cout << "��ʼ������Ϣ" << endl;
	cout << "��Ϣ������" << crackInfos.size() << endl;

	LUT(resImg, lookUpTable, resImg);
	for (auto domain_it = connectedDomains.cbegin(); domain_it != connectedDomains.cend(); ++domain_it)
	{
		for (auto point_it = domain_it->cbegin(); point_it != domain_it->cend(); ++point_it)
		{
			resImg.ptr<uchar>(point_it->y)[point_it->x] = 255;
		}
	}

	resImg = ~resImg; //��ɫȡ������ȡ�׵׺ڷ��ͼ��[ssr 07-09]
	imwrite(GLOBAL_OUTPUT_DIR + "result1.png", resImg);
	Mat redRes = imread(GLOBAL_OUTPUT_DIR + "result1.png");

	CvGeFunctions::turnBlackCrackToRed(redRes);//�ѷ���ɫȡ��ɫ[ssr 07-09]
	imwrite(GLOBAL_OUTPUT_DIR + "redRes.png", redRes);

	Mat mix;
	addWeighted(redRes, 0.5, orgImg, 0.5, 0.0, mix);//�ѷ�ͼ��ԭͼ��1:1Ȩ�ػ�� [ssr 07-09]
	imwrite(GLOBAL_OUTPUT_DIR + "mixRes.png", mix);

	cout << "����ͼ�����" << endl;
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
ErrorStatus CvProgramFunctions::getCrackLengthByRatio(vector<vector<Point>>& vecCracks, multimap<double, Point>& mapLength_Pos, double changeRatio)
{
	for (int i = 0; i < vecCracks.size(); ++i)
	{
		int pixelLength = vecCracks[i].size();
		Point crPos = vecCracks[i][pixelLength / 2];
		double trueLength = pixelLength * changeRatio;
		mapLength_Pos.insert(std::make_pair( trueLength, crPos));
	}
	return SUCCEED;
}
