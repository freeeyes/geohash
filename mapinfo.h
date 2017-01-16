#ifndef _MAPINFO_H
#define _MAPINFO_H

#include "HashTable.h"
#include "geohash.h"
#include "posinfopool.h"
#include "areainfopool.h"

//中国地图范围内的所有区域，按照500米的范围分割，并存入共享内存。
//add by freeeyes

#define GEO_PERSITION 12   

class CMapInfo
{
public:
	CMapInfo();
	~CMapInfo();
	
	void Init(char* pData);
	void Load(char* pData);
	
	bool AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos);
	
	size_t GetSize(int nMaxCount);
	
private:
	void prepareCryptTable();	
	
private:	
	char*         m_pCryptTable;     //hash词典算法词
	CHashTable    m_objHashArea;     //Hash区域地图 
	CHashTable    m_objHashCurrPos;  //Hash区域地图
	int           m_AreaCount;       //所有的区域个数 
	CPosInfoPool  m_objPosInfoList;  //当前点对象池
	CAreaInfoPool m_objAreaInfoList; //当前区域对象池 
};

#endif
