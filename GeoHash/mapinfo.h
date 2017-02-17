#ifndef _MAPINFO_H
#define _MAPINFO_H

#include "imapinfo.h"
#include "HashTable.h"
#include "geohash.h"
#include "poslinkpool.h"
#include "areainfopool.h"

//中国地图范围内的所有区域，按照500米的范围分割，并存入共享内存。
//add by freeeyes

#define GEO_PERSITION 12   

class CMapInfo : public IMapInfo
{
public:
	CMapInfo();
	~CMapInfo();
	
	void Init(char* pData);
	void Load(char* pData);
	
	bool AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos);
	
	bool FindPos(double dPosLatitude, double dPosLongitude, double dDistance, vector<_Pos_Info*>& vecPosList);
	
	bool DelPos(const char* pMsisdn);
	
	size_t GetSize(int nMaxCount);
	
private:
	void prepareCryptTable();	
	
	void GetNeighbors(double dPosLatitude, double dPosLongitude, double dDistance, vector<string>& objNeighborsList);
	
private:	
	char*         m_pCryptTable;     //hash词典算法词
	CHashTable    m_objHashArea;     //Hash区域地图 
	CHashTable    m_objHashCurrPos;  //Hash区域地图
	int           m_AreaCount;       //所有的区域个数 
	CPosInfoPool  m_objPosInfoList;  //当前点对象池
	CAreaInfoPool m_objAreaInfoList; //当前区域对象池 
	CPosLinkPool  m_objPosLinkList;  //当前点链表对象池 
};

#endif
