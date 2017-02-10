#ifndef _POSLINKPOOL_H
#define _POSLINKPOOL_H

#include "common.h"

//add by freeeyes
//此内存池用于构建链表指针的存储

class CPosLinkPool
{
public:
	CPosLinkPool();
	~CPosLinkPool();
	
	void Close();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);	
	
	_PosLink_Info* Create();
	bool Delete(_PosLink_Info* pPosLinkInfo);
	
	_PosLink_Info* Get(int nIndex);
	
private:
	char*          m_pBase;          //整个内存开始地址
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_PosLink_Info* m_PosLinkList;    //内存池列表			
};

#endif
