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
	
	_Pool_Info Get_Pool_State();
	
	size_t GetSize(int nCount);
	
private:
	char*          m_pBase;          //整个内存开始地址
	int*           m_pUsedCount;     //当前内存池已经使用对象个数
	int*           m_pPoolCount;     //当前池中对象总数
	int*           m_pCurrIndex;     //当前节点位置
	_PosLink_Info* m_PosLinkList;    //内存池列表			
};

#endif
