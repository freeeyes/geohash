#include "geohash.h"

CGeoHash::CGeoHash()
{
	//默认精度为20
	Init(20);
}

CGeoHash::CGeoHash(int nPrecision)
{
	Init(nPrecision);
}

CGeoHash::~CGeoHash()
{
	Close();
}

void CGeoHash::Init(int nPrecision)
{
	sprintf(m_szBase32, "0123456789bcdefghjkmnpqrstuvwxyz");
	m_szBase32[32]   = '\0';
	m_nBase32Len     = 32;
	m_nPrecision     = nPrecision;
	
	if(nPrecision % 5 != 0)
	{
		if((nPrecision + nPrecision + 1) % 5 != 0)
		{
			Init(20);
		}
	}
}

void CGeoHash::Close()
{
}

void CGeoHash::GetBits(double dData, double& dMin, double& dMax, char* pData, int nPrecision)
{
	//printf("[CGeoHash::GetBits](%f)\n", dData);
	//计算bits
	double dMid = 0;
	for(int i = 0; i < nPrecision; i++)
	{
		dMid = (dMin + dMax) / 2;
		if(dData > dMid)
		{
			pData[i] = '1';
			dMin     = dMid;
		}
		else
		{
			pData[i] = '0';
			dMax     = dMid;
		}
		//printf("%c", pData[i]);
	}
	
	//printf("\n");
}

int CGeoHash::GetBase32Index(char* pData, int nBegin, int nEnd)
{
	int nRet = 0;
	double x = 2;
	int nPos = 4;
	for(int i = nBegin; i < nEnd; i++)
	{
		if(pData[i] == '1')
		{
			//printf("[CGeoHash::GetBase32Index]x=%d,nPos=%d,+=%d.\n", (int)x, nPos, (int)pow(x, nPos));
			nRet += (int)pow(x, nPos);
		}
		nPos--;
	}
	
	return nRet;
}

double CGeoHash::deg_rad(double ang)
{
	return ang / D_R;
}

double CGeoHash::rad_deg(double ang)
{
	return ang * D_R;
}

char* CGeoHash::Encode(double dLatitude, double dLongitude)
{
	double LatitudeMin  = -90.0f;
	double LatitudeMax  = 90.0f;
	double LongitudeMin = -180.0f;
	double LongitudeMax = 180.0f;
	
	//保证计算出的字符串必须能被5整除
	if(m_nPrecision % 5 == 0)
	{
		//计算出对应Bits
		GetBits(dLatitude, LatitudeMin, LatitudeMax, m_szLatitudeBits, m_nPrecision);
		GetBits(dLongitude, LongitudeMin, LongitudeMax, m_szLongitudeBits, m_nPrecision);
	}
	else
	{
		//计算出对应Bits
		GetBits(dLatitude, LatitudeMin, LatitudeMax, m_szLatitudeBits, m_nPrecision);
		GetBits(dLongitude, LongitudeMin, LongitudeMax, m_szLongitudeBits, m_nPrecision + 1);		
	}
	
	int nPosBegin     = 0;
	int nPosEnd       = nPosBegin + 5;
	int nPos = 0;
	
	//计算出组合码，并计算出对应Base32编码
	for(int i = 0; i < m_nPrecision; i++)
	{
		m_szComboBits[i*2]     = m_szLongitudeBits[i];
		m_szComboBits[i*2 + 1] = m_szLatitudeBits[i];
		
		if(i*2 + 1 >= nPosEnd)
		{
			int nIndex = GetBase32Index(m_szComboBits, nPosBegin, nPosEnd);
			nPosBegin = nPosEnd;
			nPosEnd   = nPosBegin + 5;
			m_szGeoHash[nPos] = m_szBase32[nIndex];
			nPos++;
			//printf("[CGeoHash::Get_GeoHash_String](%d)nPosEnd=%d, Data=%c.\n", nPos, nPosEnd, m_szBase32[nIndex]);
		}
	}
	
	//保证计算出的字符串必须能被5整除
	if(m_nPrecision % 5 != 0)
	{
		m_szComboBits[m_nPrecision*2] = m_szLatitudeBits[m_nPrecision + 1];
	}	
	
	int nIndex = GetBase32Index(m_szComboBits, nPosBegin, nPosEnd);
	nPosBegin = nPosEnd;
	nPosEnd   = nPosBegin + 5;
	m_szGeoHash[nPos] = m_szBase32[nIndex];
	nPos++;	
	m_szGeoHash[nPos] = '\0';
	return m_szGeoHash;
}

char* CGeoHash::Encode(double dLatitude, double dLongitude, int nPrecision)
{
	double LatitudeMin  = -90.0f;
	double LatitudeMax  = 90.0f;
	double LongitudeMin = -180.0f;
	double LongitudeMax = 180.0f;	
	
	memset(m_szComboBits, 0, 100);
	memset(m_szLatitudeBits, 0, 50);
	memset(m_szLongitudeBits, 0, 50);
	
	if(nPrecision % 5 != 0)
	{
		if((nPrecision + nPrecision + 1) % 5 != 0)
		{
			return NULL;
		}
	}	
	
	//保证计算出的字符串必须能被5整除
	if(nPrecision % 5 == 0)
	{
		//计算出对应Bits
		GetBits(dLatitude, LatitudeMin, LatitudeMax, m_szLatitudeBits, nPrecision);
		GetBits(dLongitude, LongitudeMin, LongitudeMax, m_szLongitudeBits, nPrecision);
	}
	else
	{
		//计算出对应Bits
		GetBits(dLatitude, LatitudeMin, LatitudeMax, m_szLatitudeBits, nPrecision);
		GetBits(dLongitude, LongitudeMin, LongitudeMax, m_szLongitudeBits, nPrecision + 1);		
	}
	
	int nPos          = 0;
	int nAllComboLen  = 0;
	
	//计算出组合码，并计算出对应Base32编码
	for(int i = 0; i < nPrecision; i++)
	{
		m_szComboBits[i*2]     = m_szLongitudeBits[i];
		m_szComboBits[i*2 + 1] = m_szLatitudeBits[i];
	}
	
	//如果纬度比经度多1
	if(nPrecision % 5 != 0)
	{
		m_szComboBits[nPrecision*2] = m_szLongitudeBits[nPrecision];
		nAllComboLen = nPrecision*2 + 1;
	}
	else
	{
		nAllComboLen = nPrecision*2; 
	}
	
	//按5位一换数据
	//printf("[A](%d)m_szLongitudeBits=%s.\n", nPrecision + 1, m_szLongitudeBits);
	//printf("[A](%d)m_szLatitudeBits=%s.\n", nPrecision, m_szLatitudeBits);
	//printf("[A](%d)m_szComboBits=%s.\n", nAllComboLen, m_szComboBits);
	for(int i = 0; i < nAllComboLen; i = i + 5)
	{
		int nIndex = GetBase32Index(m_szComboBits, i, i + 5);
		m_szGeoHash[nPos] = m_szBase32[nIndex];
		nPos++;
	}

	m_szGeoHash[nPos] = '\0';
	return m_szGeoHash;	
}

_Geo_Rect CGeoHash::GetGeoRect(double dLatitude, double dLongitude, int nPrecision)
{
	_Geo_Rect obj_Geo_Rect;
	obj_Geo_Rect.m_dMinLatitude  = -90.0f;
	obj_Geo_Rect.m_dMaxLatitude  = 90.0f;
	obj_Geo_Rect.m_dMinLongitude = -180.0f;
	obj_Geo_Rect.m_dMaxLongitude = 180.0f;
	
	if(nPrecision % 5 != 0)
	{
		if((nPrecision + nPrecision + 1) % 5 != 0)
		{
			return obj_Geo_Rect;
		}
	}	
	
	//保证计算出的字符串必须能被5整除
	if(nPrecision % 5 == 0)
	{
		//计算出对应Bits
		GetBits(dLatitude, obj_Geo_Rect.m_dMinLatitude, obj_Geo_Rect.m_dMaxLatitude, m_szLatitudeBits, nPrecision);
		GetBits(dLongitude, obj_Geo_Rect.m_dMinLongitude, obj_Geo_Rect.m_dMaxLongitude, m_szLongitudeBits, nPrecision);
	}
	else
	{
		//计算出对应Bits
		GetBits(dLatitude, obj_Geo_Rect.m_dMinLatitude, obj_Geo_Rect.m_dMaxLatitude, m_szLatitudeBits, nPrecision);
		GetBits(dLongitude, obj_Geo_Rect.m_dMinLongitude, obj_Geo_Rect.m_dMaxLongitude, m_szLongitudeBits, nPrecision + 1);		
	}
	
	return obj_Geo_Rect;
}

_Geo_Neighbors CGeoHash::GetNeighbors(double dLatitude, double dLongitude, int nPrecision)
{
	//得到对应位置的九宫格
	_Geo_Neighbors obj_Geo_Neighbors;
	
	_Geo_Rect obj_Geo_Rect = GetGeoRect(dLatitude, dLongitude, nPrecision);
	sprintf(obj_Geo_Neighbors.m_szNerghbors[0], "%s", Encode(dLatitude, dLongitude, nPrecision));
	
	//上
	sprintf(obj_Geo_Neighbors.m_szNerghbors[1], "%s", Encode((obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude)/2 + (obj_Geo_Rect.m_dMaxLatitude - obj_Geo_Rect.m_dMinLatitude),
																						               (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude)/2, 
																						               nPrecision));
	//下
	sprintf(obj_Geo_Neighbors.m_szNerghbors[2], "%s", Encode((obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude)/2 - (obj_Geo_Rect.m_dMaxLatitude - obj_Geo_Rect.m_dMinLatitude),
																						               (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude)/2, 
																						               nPrecision));	
	//左
	sprintf(obj_Geo_Neighbors.m_szNerghbors[3], "%s", Encode((obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude)/2,
																						               (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude)/2 - (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude), 
																						               nPrecision));
	//右
	sprintf(obj_Geo_Neighbors.m_szNerghbors[4], "%s", Encode((obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude)/2,
																						               (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude)/2 + (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude), 
																						               nPrecision));
	//左下
	sprintf(obj_Geo_Neighbors.m_szNerghbors[5], "%s", Encode((obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude)/2 - (obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude),
																						               (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude)/2 - (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude), 
																						               nPrecision));
	//右下
	sprintf(obj_Geo_Neighbors.m_szNerghbors[6], "%s", Encode((obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude)/2 + (obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude),
																						 							 (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude)/2 - (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude), 
																						               nPrecision));
	//左上
	sprintf(obj_Geo_Neighbors.m_szNerghbors[7], "%s", Encode((obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude)/2 - (obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude),
																						               (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude)/2 + (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude), 
																						               nPrecision));	
	//右上
	sprintf(obj_Geo_Neighbors.m_szNerghbors[8], "%s", Encode((obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude)/2 + (obj_Geo_Rect.m_dMaxLatitude + obj_Geo_Rect.m_dMinLatitude),
																						               (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude)/2 + (obj_Geo_Rect.m_dMaxLongitude + obj_Geo_Rect.m_dMinLongitude), 
																						               nPrecision));
	return obj_Geo_Neighbors;																	 															 	
}

double CGeoHash::GetDistance(double dLatitude, double dLongitude, double dsLatitude, double dsLongitude)
{
	double radlat1 = M_PI*dLatitude/180.0f;
	double radlat2 = M_PI*dsLatitude/180.0f;
	
	//now long.
	double radlong1 = M_PI*dLongitude/180.0f;
	double radlong2 = M_PI*dsLongitude/180.0f;
	
	if( radlat1 < 0 ) radlat1 = M_PI/2 + fabs(radlat1);// south
	if( radlat1 > 0 ) radlat1 = M_PI/2 - fabs(radlat1);// north
	if( radlong1 < 0 ) radlong1 = M_PI*2 - fabs(radlong1);//west
	if( radlat2 < 0 ) radlat2 = M_PI/2 + fabs(radlat2);// south
	if( radlat2 > 0 ) radlat2 = M_PI/2 - fabs(radlat2);// north
	if( radlong2 < 0 ) radlong2 = M_PI*2 - fabs(radlong2);// west
	
	double x1 = EARTH_RADIUS_IN_METERS * cos(radlong1) * sin(radlat1);
	double y1 = EARTH_RADIUS_IN_METERS * sin(radlong1) * sin(radlat1);
	double z1 = EARTH_RADIUS_IN_METERS * cos(radlat1);
	double x2 = EARTH_RADIUS_IN_METERS * cos(radlong2) * sin(radlat2);
	double y2 = EARTH_RADIUS_IN_METERS * sin(radlong2) * sin(radlat2);
	double z2 = EARTH_RADIUS_IN_METERS * cos(radlat2);
	double d = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
	
	double theta = acos((EARTH_RADIUS_IN_METERS*EARTH_RADIUS_IN_METERS+EARTH_RADIUS_IN_METERS*EARTH_RADIUS_IN_METERS-d*d) / (2* EARTH_RADIUS_IN_METERS * EARTH_RADIUS_IN_METERS));
	double dist  = theta * EARTH_RADIUS_IN_METERS;
	
	return dist;	 
}

_Geo_Rect CGeoHash::GetBoundingBox(double dLatitude, double dLongitude, double dRadiusMeters)
{
	_Geo_Rect obj_Geo_Rect;
	
  obj_Geo_Rect.m_dMinLongitude = dLongitude - rad_deg(dRadiusMeters/EARTH_RADIUS_IN_METERS/cos(deg_rad(dLatitude)));
  obj_Geo_Rect.m_dMaxLongitude = dLongitude + rad_deg(dRadiusMeters/EARTH_RADIUS_IN_METERS/cos(deg_rad(dLatitude)));
  obj_Geo_Rect.m_dMinLatitude  = dLatitude - rad_deg(dRadiusMeters/EARTH_RADIUS_IN_METERS);
  obj_Geo_Rect.m_dMaxLatitude  = dLatitude + rad_deg(dRadiusMeters/EARTH_RADIUS_IN_METERS);	
  
  return obj_Geo_Rect;
}
