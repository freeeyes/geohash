#ifndef _MAPINFO_H
#define _MAPINFO_H

#include "HashTable.h"
#include "geohash.h"
#include "time.h"

//中国地图范围内的所有区域，按照500米的范围分割，并存入共享内存。
//add by freeeyes

#define MAX_GEO_SIZE          10
#define MAX_AREA_OBJECT_COUNT 200

struct _POS_INFO
{
	char   m_szMsisdn[15];       //SIM卡号
	double m_dPosLatitude;       //纬度
	double m_dPosLongitude;      //经度
	time_t m_ttCurrTime;         //当前点时间
	char   m_cUsed;              //是否被使用，0为未被使用，1为已使用 
	int    m_nIndex;             //当前的Pool对象ID
	
	void Init()
	{
		m_dPosLatitude   = 0.0f;
		m_dPosLongitude  = 0.0f;
		m_cUsed          = 0;
		m_nIndex         = 0;
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}
};

class CPosInfoPool
{
public:
	CPosInfoPool();
	~CPosInfoPool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	char* GetCryptTable();
	
	_POS_INFO* Create();
	int Get_Node_Offset(_POS_INFO* pWordInfo);
	_POS_INFO* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_POS_INFO* pWordInfo);
	
private:
	char*          m_pBase;           //整个内存开始地址
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_POS_INFO*     m_PosInfoList;    //内存池列表			
};

struct _Area_Info
{
	char       m_szHashGeo[MAX_GEO_SIZE];
	int        m_objPosList[MAX_AREA_OBJECT_COUNT];
	
	void Init()
	{
		m_szHashGeo[0] = '\0';
		for(int i = 0; i < MAX_AREA_OBJECT_COUNT; i++)
		{
			//这里记录的是偏移距离
			m_objPosList[i] = -1;
		}
	}
};

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
	char*        m_pCryptTable;     //hash词典算法词
	CHashTable   m_objHashArea;     //Hash地图 
	_Area_Info*  m_pAreaInfo;       //所有区域地图
	int          m_AreaCount;       //所有的区域个数 
	CPosInfoPool m_objPosInfoList;  //当前点对象池
};

#endif
