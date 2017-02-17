#include "poslinkpool.h"

CPosLinkPool::CPosLinkPool()
{
	m_pBase       = NULL;
	m_PosLinkList = NULL;
	m_pUsedCount  = NULL;
	m_pPoolCount  = NULL;
	m_pCurrIndex  = NULL;
}

CPosLinkPool::~CPosLinkPool()
{
	Close();
}

void CPosLinkPool::Close()
{
	m_pBase         = NULL;	
	m_PosLinkList   = NULL;	
}

size_t CPosLinkPool::GetSize(int nCount)
{
	return sizeof(_PosLink_Info) * nCount + sizeof(int) * 3;
}

size_t CPosLinkPool::Init(int nPoolCount, char* pData)
{
	Close();
	
	size_t nPos  = 0;
	
	m_pBase      = pData;
	m_pPoolCount = (int* )&pData[nPos];
	nPos += sizeof(int);
	m_pUsedCount = (int* )&pData[nPos];
	nPos += sizeof(int);
	m_pCurrIndex = (int* )&pData[nPos];
	nPos += sizeof(int);
	
	//printf("[CAreaInfoPool::Init]nPos=%d.\n", nPos);
	m_PosLinkList = (_PosLink_Info* )&pData[nPos];
	nPos += sizeof(_PosLink_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_PosLinkList[i].Init();
		m_PosLinkList[i].Set_Index(i);
	}
	
	(*m_pPoolCount) = nPoolCount;
	(*m_pUsedCount) = 0;
	(*m_pCurrIndex) = 0;	
	
	return nPos;		
}

size_t CPosLinkPool::Load(int nPoolCount, char* pData)
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
	m_PosLinkList = (_PosLink_Info* )&pData[nPos];
	nPos += sizeof(_PosLink_Info) * nPoolCount;
	
	printf("[CPosLinkPool::Load]m_pPoolCount=%d.\n", (*m_pPoolCount));
	printf("[CPosLinkPool::Load]m_pCurrIndex=%d.\n", (*m_pCurrIndex));
	printf("[CPosLinkPool::Load]m_pUsedCount=%d.\n", (*m_pUsedCount));
	for(int i = 0; i < (*m_pPoolCount); i++)
	{
		m_PosLinkList[i].m_pNext = NULL;
		if(m_PosLinkList[i].m_cUsed == 1)
		{
			//printf("[CPosLinkPool::Load]m_PosLinkList OK.\n");
		}
	}		
	
	return nPos;			
}

_PosLink_Info* CPosLinkPool::Get(int nIndex)
{
	if(nIndex < 0 || nIndex >= (*m_pPoolCount))
	{
		return NULL;
	}
	
	return &m_PosLinkList[nIndex];
}

_PosLink_Info* CPosLinkPool::Create()
{
	if(NULL == m_PosLinkList)
	{
		return NULL;
	}
	
	if((*m_pCurrIndex) >= (*m_pPoolCount) - 1)
	{
		(*m_pCurrIndex) = 0;
	}	
	
	if(m_PosLinkList[(*m_pCurrIndex)].m_cUsed == 0)
	{
		//printf("[CPosLinkPool::Create](*m_pCurrIndex)=%d, nIndex=%d.\n", (*m_pCurrIndex), m_PosLinkList[(*m_pCurrIndex)].Get_Index());
		m_PosLinkList[(*m_pCurrIndex)].m_cUsed = 1;
		(*m_pUsedCount)++;
		return &m_PosLinkList[(*m_pCurrIndex)++];
	}
	else
	{
		//循环寻找空位
		for(int i = (*m_pCurrIndex) + 1; i < (*m_pPoolCount); i++)
		{
			if(m_PosLinkList[i].m_cUsed == 0)
			{
				(*m_pCurrIndex) = i + 1;
				if((*m_pCurrIndex) > (*m_pPoolCount) - 1)
				{
					(*m_pCurrIndex) = 0;
				}
				m_PosLinkList[i].m_cUsed = 1;
				//printf("[CPosLinkPool::Create](*m_pCurrIndex)=%d, nIndex=%d.\n", (*m_pCurrIndex), m_PosLinkList[(*m_pCurrIndex)].Get_Index());
				(*m_pUsedCount)++;
				return &m_PosLinkList[i];				
			}
		}
		
		//printf("[CPosLinkPool::Create](*m_pCurrIndex)=%d,(*m_pPoolCount)=%d.\n", (*m_pCurrIndex), (*m_pPoolCount));
		int nStart = 0;
		//没找到，再重头开始找
		for(int i = nStart; i < (*m_pCurrIndex) - 1; i++)
		{
			if(m_PosLinkList[i].m_cUsed == 0)
			{
				(*m_pCurrIndex) = i + 1;
				m_PosLinkList[i].m_cUsed = 1;
				//printf("[CPosLinkPool::Create](*m_pCurrIndex)=%d, nIndex=%d.\n", (*m_pCurrIndex), m_PosLinkList[(*m_pCurrIndex)].Get_Index());
				(*m_pUsedCount)++;
				return &m_PosLinkList[i];				
			}			
		}
		
		//已经没有空位
		return NULL;
	}			
}

bool CPosLinkPool::Delete(_PosLink_Info* pPosLinkInfo)
{
	if(NULL == m_PosLinkList || NULL == pPosLinkInfo)
	{
		return false;
	}	
	
	if(-1 == pPosLinkInfo->Get_Index())
	{
		return false;
	}
	
	if(pPosLinkInfo->Get_Index() >= (*m_pPoolCount) || pPosLinkInfo->Get_Index() < 0)
	{
		printf("[CAreaInfoPool::Delete]Get_Index=%d is unvalid.\n", pPosLinkInfo->Get_Index());
		return false;
	}
	
	m_PosLinkList[pPosLinkInfo->Get_Index()].Clear();
	(*m_pUsedCount)--;
	return true;		
}

_Pool_Info CPosLinkPool::Get_Pool_State()
{
	_Pool_Info obj_Pool_Info;
	obj_Pool_Info.m_nUsedCount = (*m_pUsedCount);
	obj_Pool_Info.m_nPoolCount = (*m_pPoolCount);
	//printf("[CPosLinkPool::Get_Pool_State](*m_pUsedCount)=%d,(*m_pPoolCount)=%d.\n", (*m_pUsedCount), (*m_pPoolCount));
	return obj_Pool_Info;	
}
