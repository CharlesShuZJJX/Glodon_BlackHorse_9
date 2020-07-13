#include"CommandsGraphic.h"

void Cmd::cmdOutPutCrackGraph()
{
	Mat orgImg = imread(GLOBAL_ORIGIN_JPG_DIR);
	Mat resImg;
	vector<vector<Point>> connectedDomains;
	int errorStatue = CvProgramFunctions::getConnectedDomain(connectedDomains, orgImg, resImg);
	if (errorStatue == ERROR_OPENFILE)
	{
		printf("´ò¿ªÍ¼Æ¬Ê§°Ü£¡");
	}
	CvProgramFunctions::showCracks(connectedDomains, orgImg, resImg);
}
void Cmd::cmdRenewPixelToRealRatio()
{
	switch (GLOBAL_GET_RATIO_METHOD)
	{
	case BY_DISTANCE:
	{
		easyexif::EXIFInfo result;
		double focal = 0.0;
		double focal35mm = 0.0;
		easyexif::ExifFunctions::cmdGetExifValue(GLOBAL_ORIGIN_JPG_DIR.c_str(), result);
		easyexif::ExifFunctions::getFocus(result, focal, focal35mm);
		GLOBAL_RCMOS = easyexif::ExifFunctions::getRcmos(focal, focal35mm);
		GLOBAL_CAMERA_FOCAL = focal;
		GLOBAL_35MM_CAMERA_FOCAL = focal35mm;
		GLOBAL_PIXEL_TO_REAL_RATIO = CvGeFunctions::getChangeRatio(GLOBAL_RCMOS, focal, focal35mm);
		break;
	}
	case BY_REFRENCE:
	default:
	{
		GLOBAL_PIXEL_TO_REAL_RATIO = CvGeFunctions::getChangeRatio(GLOBAL_REF_PIEXL_LEN, GLOBAL_REF_REAL_LEN);
		break;
	}
	}
}