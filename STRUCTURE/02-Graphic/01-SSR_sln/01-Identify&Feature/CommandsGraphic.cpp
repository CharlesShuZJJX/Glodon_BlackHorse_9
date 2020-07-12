#include"CommandsGraphic.h"

void Cmd::cmdOutPutCrackGraph()
{
	Mat orgImg = imread(GLOBAL_ORIGIN_JPG_DIR);
	Mat resImg;
	vector<vector<Point>> connectedDomains;
	int errorStatue = CvProgramFunctions::getConnectedDomain(connectedDomains, orgImg, resImg);
	if (errorStatue == ERROR_OPENFILE)
	{
		printf("��ͼƬʧ�ܣ�");
	}
	CvProgramFunctions::showCracks(connectedDomains, orgImg, resImg);
}