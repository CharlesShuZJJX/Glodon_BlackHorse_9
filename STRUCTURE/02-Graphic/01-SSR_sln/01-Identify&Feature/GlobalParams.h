//һЩȫ�ֱ���,��ͷ�ļ�����������cpp�ж���
#pragma once
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

/*   01 - ִ�г���������             */
/**/ typedef int ErrorStatus;
/**/ enum { SUCCEED, FAILED, ERROR_OPENFILE, ERROR_WRITEFILE };
/////////////////////////////////////////////////////////////////////////////////////////////

/*   02 - ͼƬ���Ŀ¼                 */
/**/ extern const string GLOBAL_OUTPUT_DIR;
////////////////////////////////////////////////////////////////////////////////////////////

/*   03 - ͼƬ����Ŀ¼                 */
/**/ extern const string GLOBAL_ORIGIN_JPG_DIR;
/////////////////////////////////////////////////////////////////////////////////////////////

/*   04 - ������ʵ�ʳ��ȵ�ת��ϵ��     */
/**/ extern double GLOBAL_PIXEL_TO_REAL_RATIO;
/////////////////////////////////////////////////////////////////////////////////////////////

/*   05 - �������                    */
/**/ extern double GLOBAL_CAMERA_FOCAL;
/////////////////////////////////////////////////////////////////////////////////////////////

/*   06 - ���35mm��Ч����            */
/**/ extern double GLOBAL_35MM_CAMERA_FOCAL;
/////////////////////////////////////////////////////////////////////////////////////////////

/*   07 - ���CMOS�ĶԽ��߳ߴ�        */
/**/ extern double GLOBAL_RCMOS;
/////////////////////////////////////////////////////////////////////////////////////////////

/*   08 - ��ȡת��ϵ���ķ�ʽ          */
/**/ enum { BY_DISTANCE, BY_REFRENCE };
/**/ extern int GLOBAL_GET_RATIO_METHOD;
/////////////////////////////////////////////////////////////////////////////////////////////

/*   09 - ���ղ�෨�Ĳ���           */
/**/ extern double GLOBAL_REF_PIEXL_LEN;
/**/ extern double GLOBAL_REF_REAL_LEN;
/////////////////////////////////////////////////////////////////////////////////////////////

/*   10 - Canny�õ��ı�Ե��          */
/**/ extern vector<Point> GLOBAL_OUTPTS;
/////////////////////////////////////////////////////////////////////////////////////////////

/*   11 - �����ѷ���Ϣ��             */
class crackInterface {
public:
	crackInterface() {};
	crackInterface(vector<Point> vecDomain, vector<Point> vecSkl)
	{
		m_vecDomain = vecDomain;
		m_vecSkl = vecSkl;
		m_Index = m_numCreated;
		m_length = 0;
		m_width = 0;
		m_area = 0;
		++m_numCreated;
	}
public:
	void   setLength(double len) { m_length = len; }
	double getLength() { return m_length; }
	void   setWidth(double wid) { m_width = wid; }
	double getWidth() { return m_width; }
	void   setArea(double area) { m_area = area; }
	double getArea() { return m_area; }
public:
	vector<Point> m_vecDomain;
	vector<Point> m_vecSkl;
private:
	static int m_numCreated;
	int m_Index;
	double m_length;
	double m_width;
	double m_area;
	Point m_pos;
};
extern vector<crackInterface> GLOBAL_VEC_CRACK;
/////////////////////////////////////////////////////////////////////////////////////////////