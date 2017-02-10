#ifndef _POSINFOPOOL_H
#define _POSINFOPOOL_H

//定位点内存池
//add by freeeyes
#include "common.h"

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
