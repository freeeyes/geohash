#include "mapinfo.h"
#include "ShareMemory.h"

//g++ -o Test geohash.cpp main.cpp
int main()
{
	CGeoHash m_GeoHash;
	
	/*
	printf("[main]geohash1=%s.\n", m_GeoHash.Encode(39.928167, 116.389550, 12));
	//printf("[main]geohash2=%s.\n", m_GeoHash.Get_GeoHash_String(39.928167, 116.389540));
	//printf("[main]geohash3=%s.\n", m_GeoHash.Get_GeoHash_String(39.928177, 116.389550));86.86385
	
	_Geo_Rect obj_Geo_Rect = m_GeoHash.GetGeoRect(39.928167, 116.389550, 20);
	printf("[main]m_dMinLatitude=%f, m_dMaxLatitude=%f.\n", obj_Geo_Rect.m_dMinLatitude, obj_Geo_Rect.m_dMaxLatitude);
	printf("[main]m_dMinLongitude=%f, m_dMaxLongitude=%f.\n", obj_Geo_Rect.m_dMinLongitude, obj_Geo_Rect.m_dMaxLongitude);
	
	
	_Geo_Neighbors obj_Geo_Neighbors;
	obj_Geo_Neighbors = m_GeoHash.GetNeighbors(39.928167, 116.389550, 20);
	printf("[main]Neighbors(");
	for(int i= 0; i < 9; i ++)
	{
		printf("%s,", obj_Geo_Neighbors.m_szNerghbors[i]);
	}	
	printf(");\n");
	
	
	_Geo_Rect obj_Geo_Rect;
	
	obj_Geo_Rect.m_dMinLatitude  = 53.3;
	obj_Geo_Rect.m_dMinLongitude = 73.4;
	obj_Geo_Rect.m_dMaxLatitude  = 53.3;
	obj_Geo_Rect.m_dMaxLongitude = 136.3;
	
	
	double dDistance = m_GeoHash.GetDistance(obj_Geo_Rect.m_dMinLatitude, obj_Geo_Rect.m_dMinLongitude, 
																					 obj_Geo_Rect.m_dMaxLatitude, obj_Geo_Rect.m_dMaxLongitude);
	printf("[main]distance=%f.\n", dDistance);
	*/
	
	CMapInfo objMapInfo;
	size_t stShareSize = objMapInfo.GetSize(1000000);
	printf("[main]All Size=%d.\n", stShareSize); 
	
	int nPoolSize = 600000;
	shm_key obj_key = 30001;
	shm_id obj_shm_id;
	bool blCreate = true;		
	char* pData = Open_Share_Memory_API(obj_key, stShareSize, obj_shm_id, blCreate);
	//char* pData = new char[stShareSize];
	
	if(NULL != pData)
	{
		if(blCreate == true)
		{
			memset(pData, 0, stShareSize);
			objMapInfo.Init(pData);
		}
		else
		{
			objMapInfo.Load(pData);
		}
	}
	else
	{
		printf("[main]Create share memory is fail.\n");
		return 0;
	}
	
	//测试第一个点
	time_t ttNow = time(NULL);
	objMapInfo.AddPos("13661201023", 39.928167, 116.389550, ttNow);
	
	//测试第二个点
	objMapInfo.AddPos("13661201023", 37.928167, 116.389550, ttNow);
	
	//delete[] pData;
	return 0;
}
