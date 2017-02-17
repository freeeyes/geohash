#ifndef _AREAINFOPOOL_H
#define _AREAINFOPOOL_H

//区域内存池
//add by freeeyes
#include "common.h"

class CAreaInfoPool
{
public:
	CAreaInfoPool();
	~CAreaInfoPool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	size_t GetSize(int nCount);
	
	_Area_Info* Create();
	int Get_Node_Offset(_Area_Info* pWordInfo);
	_Area_Info* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_Area_Info* pWordInfo);
	
	_Area_Info* Get(int nIndex);
	
	_Pool_Info Get_Pool_State();
	
private:
	char*          m_pBase;           //整个内存开始地址
	int*           m_pUsedCount;      //当前内存池已经使用对象个数 
	int*           m_pPoolCount;      //内存池当前总数
	int*           m_pCurrIndex;      //当前节点位置
	_Area_Info*    m_AreaInfoList;    //内存池列表			
};


#endif
