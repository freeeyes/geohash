#ifndef GEO_COMMON_H
#define GEO_COMMON_H

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "stdio.h"
#include "time.h"

//程序需要的结构体都在这里定义
//add by freeeyes

//pos部分
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


//area部分
#define MAX_GEO_SIZE          10

struct _Area_Info
{
	pthread_rwlock_t     m_rwLock;                     //区域读写锁
	char                 m_szHashGeo[MAX_GEO_SIZE];    //geo字符串
	_PosLink_Info*       m_pPosList;                   //Pos链表信息
	char                 m_cUsed;                      //是否被使用，0为未被使用，1为已使用 
	int                  m_nIndex;                     //当前的Pool对象ID
	
	void Init()
	{
		m_szHashGeo[0] = '\0';
		m_pPosList     = NULL;
		
		pthread_rwlockattr_t rwLockAttr;                 
		//初始化读写锁
		if(0 != pthread_rwlockattr_init(&rwLockAttr))
		{
			printf("[_Area_Info::Init]init rwlockattr attribute is failed ! erro reason is %s!\n", strerror(errno));
		}
		
    if(pthread_rwlockattr_setpshared(&rwLockAttr, PTHREAD_PROCESS_SHARED)!=0)
    {
        printf("[_Area_Info::Init]set rw lockattrite value is failed ! error reason is %s!\n",strerror(errno));
    }
    if(pthread_rwlock_init(&m_rwLock,&rwLockAttr)!=0)
    {
        printf("[_Area_Info::Init]init rw lock  attribute is failed !error reason is %s\n",strerror(errno));
    }	
	}
	
	void Load()
	{
		m_pPosList = NULL;
		//系统重启后释放一次写锁，防止系统崩溃后
		//pthread_rwlock_unlock(&m_rwLock);
		pthread_rwlock_destroy(&m_rwLock);
		
		pthread_rwlockattr_t rwLockAttr;                 
		//初始化读写锁
		if(0 != pthread_rwlockattr_init(&rwLockAttr))
		{
			printf("[_Area_Info::Init]init rwlockattr attribute is failed ! erro reason is %s!\n", strerror(errno));
		}
		
    if(pthread_rwlockattr_setpshared(&rwLockAttr, PTHREAD_PROCESS_SHARED)!=0)
    {
        printf("[_Area_Info::Init]set rw lockattrite value is failed ! error reason is %s!\n",strerror(errno));
    }
    if(pthread_rwlock_init(&m_rwLock,&rwLockAttr)!=0)
    {
        printf("[_Area_Info::Init]init rw lock  attribute is failed !error reason is %s\n",strerror(errno));
    }			
	}
	
	void Clear()
	{
		m_szHashGeo[0] = '\0';
		m_cUsed        = 0;
		
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}
	
	_PosLink_Info* Get(int nPosOffset)
	{
		if(m_pPosList == NULL)
		{
			//如果是链表的第一个
			return NULL;
		}
		else
		{
			_PosLink_Info* pTail = m_pPosList;
			while(pTail != NULL)
			{
				//printf("[Get]pTail->m_nPosOffset=%d.\n", pTail->m_nPosOffset);
				if(pTail->m_nPosOffset == nPosOffset)
				{
					return pTail;
				}
				
				pTail = pTail->m_pNext;
			}
		}
		
		return NULL;	
	}
	
	bool Delete(_PosLink_Info* pPosInfo)
	{
		if(m_pPosList == NULL)
		{
			return false;
		}
		else
		{
			_PosLink_Info* pBefore = NULL;
			_PosLink_Info* pTail   = m_pPosList;
			
			//遍历链表，找到相等的对象
			while(pTail != NULL)
			{
				if(pTail == pPosInfo)
				{
					//printf("[Delete]m_pPosList=0x%08x.\n", m_pPosList);
					//printf("[Delete]pPosInfo=0x%08x.\n", pPosInfo);
					//printf("[Delete]pBefore=0x%08x.\n", pBefore);
					//printf("[Delete]pTail=0x%08x.\n", pTail);
					//printf("[_Area_Info::Delete]pTail=0x%08x.\n", pTail);
					if(NULL == pBefore)
					{
						//是第一个
						m_pPosList = m_pPosList->m_pNext;
						return true;
					}
					else
					{
						//是中间的一个
						pBefore->m_pNext = pTail->m_pNext;
						return true;
					}
				}
				
				pBefore = pTail;
				pTail   = pTail->m_pNext;
			}
			
		}
		
		return false;
	}
	
	bool Add(_PosLink_Info* pPosInfo, char* pHashGeo)
	{
		sprintf(m_szHashGeo, "%s", pHashGeo);
		if(m_pPosList == NULL)
		{
			//如果是链表的第一个
			m_pPosList = pPosInfo;
		}
		else
		{
			_PosLink_Info* pTail = m_pPosList;
			while(NULL != pTail->m_pNext)
			{
				pTail = pTail->m_pNext;
			}
			
			pTail->m_pNext = pPosInfo;
		}
		
		return false;		
	}
};


#endif
