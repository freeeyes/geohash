#ifndef _POSLINKPOOL_H
#define _POSLINKPOOL_H

#include "stdio.h"

//add by freeeyes
//此内存池用于构建链表指针的存储

//链表对象
struct _PosLink_Info
{
	int  m_nPosOffset;              //Pos对象的指针偏移值
	int  m_nIndex;                  //当前对象编号
	int  m_nAreaIndex;              //当前区域编号
	char m_cUsed;                   //是否被使用，0为未被使用，1为已使用 
	_PosLink_Info* m_pNext;         //Next
	
	
	_PosLink_Info()
	{
		Init();
	}
	
	void Init()
	{
		m_nPosOffset   = 0;
		m_nIndex       = 0;
		m_nAreaIndex   = 0;
		m_cUsed        = 0;
		m_pNext        = NULL;		
	}
	
	void Clear()
	{
		m_nPosOffset   = 0;
		m_nAreaIndex   = 0;
		m_cUsed        = 0;
		m_pNext        = NULL;			
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
