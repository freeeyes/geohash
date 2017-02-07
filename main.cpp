#include "mapinfo.h"
#include "ShareMemory.h"

//g++ -o Test geohash.cpp main.cpp
int main()
{
	CMapInfo objMapInfo;
	size_t stShareSize = objMapInfo.GetSize(1000000);
	printf("[main]All Size=%d.\n", stShareSize); 
	
	int nPoolSize = 600000;
	shm_key obj_key = 30010;
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
	objMapInfo.AddPos("13661201024", 39.928367, 116.389550, ttNow);
	
	vector<_Pos_Info*> vecPosList;
 	objMapInfo.FindPos(39.928367, 116.389550, 100000.0, vecPosList);
 	for(int i = 0; i < (int)vecPosList.size(); i++)
 	{
 		printf("[FindPos]m_szMsisdn=%s, m_dPosLatitude=%f, m_dPosLongitude=%f.\n", vecPosList[i]->m_szMsisdn,
 																																							 vecPosList[i]->m_dPosLatitude,
 																																							 vecPosList[i]->m_dPosLongitude);
 	}
	
	//delete[] pData;
	return 0;
}
