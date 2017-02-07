#ifndef _POSINFOPOOL_H
#define _POSINFOPOOL_H

//定位点内存池
//add by freeeyes

#include "stdio.h"
#include "time.h"

struct _Pos_Info
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
	
	_Pos_Info* Create();
	int Get_Node_Offset(_Pos_Info* pWordInfo);
	_Pos_Info* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_Pos_Info* pWordInfo);
	
private:
	char*          m_pBase;           //整个内存开始地址
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_Pos_Info*     m_PosInfoList;    //内存池列表			
};

#endif
