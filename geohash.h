#include <stdio.h>
#include <string.h>
#include <math.h>

#define D_R (M_PI / 180.0)
const double EARTH_RADIUS_IN_METERS = 6372797.560856;

//实现grohash算法的过程
//add by freeeyes

// geohash精度的设定参考 http://en.wikipedia.org/wiki/Geohash
// geohash	length	lat bits	lng bits	lat error	lng error	km error
// 1								2					3					±23				±23				±2500
// 2								5					5					± 2.8			± 5.6			±630
// 3								7					8					± 0.70		± 0.7			±78
// 4								10				10				± 0.087		± 0.18		±20
// 5								12				13				± 0.022		± 0.022		±2.4
// 6								15				15				± 0.0027	± 0.0055	±0.61
// 7								17				18				±0.00068	±0.00068	±0.076
// 8								20				20				±0.000085	±0.00017	±0.019

struct _Geo_Rect
{
	double m_dMinLatitude;
	double m_dMaxLatitude;
	double m_dMinLongitude;
	double m_dMaxLongitude;
	
	_Geo_Rect()
	{
		m_dMinLatitude  = 0.0f;
		m_dMaxLatitude  = 0.0f;
		m_dMinLongitude = 0.0f;
		m_dMaxLongitude = 0.0f;
	}
};

struct _Geo_Neighbors
{
	char m_szNerghbors[9][10];
	
	_Geo_Neighbors()
	{
		for(int i = 0; i < 9; i++)
		{
			m_szNerghbors[i][0] = '\0';
		}
	}
};

class CGeoHash
{
public:
	CGeoHash();
	CGeoHash(int nPrecision);
	~CGeoHash();
	
	//得到GeoHash计算字符串
	char* Encode(double dLatitude, double dLongitude);
	char* Encode(double dLatitude, double dLongitude, int nPrecision);
	
	//得到当前geohash的矩形
	_Geo_Rect GetGeoRect(double dLatitude, double dLongitude, int nPrecision);
	
	//得到当前geohash点所有的九宫格
	_Geo_Neighbors GetNeighbors(double dLatitude, double dLongitude, int nPrecision);
	
	//计算任意两点间的距离
	double GetDistance(double dLatitude, double dLongitude, double dsLatitude, double dsLongitude);
	
	//得到指定半径区域的矩形区域
	_Geo_Rect GetBoundingBox(double dLatitude, double dLongitude, double dRadiusMeters);
	
private:	
	void Init(int nPrecision);
	void Close();
	
	void GetBits(double dData, double& dMin, double& dMax, char* pData, int nPrecision);
	int  GetBase32Index(char* pData, int nBegin, int nEnd);
	
	double deg_rad(double ang);
	double rad_deg(double ang);
	
private:	
	char  m_szBase32[33];
	int   m_nBase32Len;
	int   m_nPrecision;      //计算矩形区域的精度
	char  m_szLatitudeBits[50];
	char  m_szLongitudeBits[50];
	char  m_szComboBits[100];
	char  m_szGeoHash[50];
};
