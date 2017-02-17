#include "areainfopool.h"

CAreaInfoPool::CAreaInfoPool()
{
	m_pBase        = NULL;
	m_AreaInfoList = NULL;
	m_pUsedCount   = NULL;
	m_pPoolCount   = NULL;
	m_pCurrIndex   = NULL;
}

CAreaInfoPool::~CAreaInfoPool()
{
	Close();
}

void CAreaInfoPool::Close()
{
	m_pBase         = NULL;	
	m_AreaInfoList  = NULL;
}

size_t CAreaInfoPool::GetSize(int nCount)
{
	return sizeof(_Area_Info) * nCount + sizeof(int) * 3;
}

size_t CAreaInfoPool::Init(int nPoolCount, char* pData)
{
	Close();
	
	size_t nPos = 0;
	
	m_pBase     = pData;
	m_pPoolCount = (int* )&pData[nPos];
	nPos += sizeof(int);
	m_pUsedCount = (int* )&pData[nPos];
	nPos += sizeof(int);
	m_pCurrIndex = (int* )&pData[nPos];
	nPos += sizeof(int);
	
	//printf("[CAreaInfoPool::Init]nPos=%d.\n", nPos);
	m_AreaInfoList = (_Area_Info* )&pData[nPos];
	nPos += sizeof(_Area_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_AreaInfoList[i].Init();
		m_AreaInfoList[i].Set_Index(i);
		//printf("[CAreaInfoPool::Init](0)nPos=%d.\n", nPos);
	}
	
	(*m_pPoolCount)   = nPoolCount;
	(*m_pUsedCount)   = 0;
	(*m_pCurrIndex)   = 0;	
	
	return nPos;	
}

size_t CAreaInfoPool::Load(int nPoolCount, char* pData)
{
	Close();
	
	size_t nPos   = 0;
	
	m_pBase       = pData;
	m_pPoolCount = (int* )&pData[nPos];
	nPos += sizeof(int);
	m_pUsedCount = (int* )&pData[nPos];
	nPos += sizeof(int);
	m_pCurrIndex = (int* )&pData[nPos];
	nPos += sizeof(int);	
	
	//printf("[CNodePool::Load]nPos=%d.\n", nPos);
	m_AreaInfoList = (_Area_Info* )&pData[nPos];
	nPos += sizeof(_Area_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_AreaInfoList[i].Load();
	}
	
	return nPos;		
}

_Area_Info* CAreaInfoPool::Get(int nIndex)
{
	if(nIndex < 0 || nIndex >= (*m_pPoolCount))
	{
		return NULL;
	}
	
	return &m_AreaInfoList[nIndex];	
}

_Area_Info* CAreaInfoPool::Create()
{
	if(NULL == m_AreaInfoList)
	{
		return NULL;
	}
	
	if((*m_pCurrIndex) >= (*m_pPoolCount) - 1)
	{
		(*m_pCurrIndex) = 0;
	}	
	
	if(m_AreaInfoList[(*m_pCurrIndex)].m_cUsed == 0)
	{
		//printf("[CAreaInfoPool::Create](*m_pCurrIndex)=%d, nIndex=%d.\n", (*m_pCurrIndex), m_AreaInfoList[(*m_pCurrIndex)].Get_Index());
		m_AreaInfoList[(*m_pCurrIndex)].m_cUsed = 1;
		(*m_pUsedCount)++;
		return &m_AreaInfoList[(*m_pCurrIndex)++];
	}
	else
	{
		//循环寻找空位
		for(int i = (*m_pCurrIndex) + 1; i < (*m_pPoolCount); i++)
		{
			if(m_AreaInfoList[i].m_cUsed == 0)
			{
				(*m_pCurrIndex) = i + 1;
				if((*m_pCurrIndex) > (*m_pPoolCount) - 1)
				{
					(*m_pCurrIndex) = 0;
				}
				m_AreaInfoList[i].m_cUsed = 1;
				(*m_pUsedCount)++;
				return &m_AreaInfoList[i];				
			}
		}
		
		//printf("[CAreaInfoPool::Create](*m_pCurrIndex)=%d,(*m_pPoolCount)=%d.\n", (*m_pCurrIndex), (*m_pPoolCount));
		int nStart = 0;
		//没找到，再重头开始找
		for(int i = nStart; i < (*m_pCurrIndex) - 1; i++)
		{
			if(m_AreaInfoList[i].m_cUsed == 0)
			{
				(*m_pCurrIndex) = i + 1;
				m_AreaInfoList[i].m_cUsed = 1;
				(*m_pUsedCount)++;
				return &m_AreaInfoList[i];				
			}			
		}
		
		//已经没有空位
		return NULL;
	}		
}

int CAreaInfoPool::Get_Node_Offset(_Area_Info* pWordInfo)
{
	int nOffset = 0;
	if(NULL != pWordInfo)
	{
		nOffset = (int)((char* )pWordInfo - m_pBase);
	}
	
	return nOffset;
}
	
_Area_Info* CAreaInfoPool::Get_NodeOffset_Ptr(int nOffset)
{
	return (_Area_Info* )(m_pBase + nOffset);
}

bool CAreaInfoPool::Delete(_Area_Info* pWordInfo)
{
	if(NULL == m_AreaInfoList || NULL == pWordInfo)
	{
		return false;
	}	
	
	if(-1 == pWordInfo->Get_Index())
	{
		return false;
	}
	
	if(pWordInfo->Get_Index() >= (*m_pPoolCount) || pWordInfo->Get_Index() < 0)
	{
		printf("[CAreaInfoPool::Delete]Get_Index=%d is unvalid.\n", pWordInfo->Get_Index());
		return false;
	}
	
	m_AreaInfoList[pWordInfo->Get_Index()].Clear();
	(*m_pUsedCount)--;
	return true;	
}

_Pool_Info CAreaInfoPool::Get_Pool_State()
{
	_Pool_Info obj_Pool_Info;
	obj_Pool_Info.m_nUsedCount = (*m_pUsedCount);
	obj_Pool_Info.m_nPoolCount = (*m_pPoolCount);
	//printf("[CAreaInfoPool::Get_Pool_State](*m_pUsedCount)=%d,(*m_pPoolCount)=%d.\n", (*m_pUsedCount), (*m_pPoolCount));
	return obj_Pool_Info;	
}
