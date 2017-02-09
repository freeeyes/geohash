#include "poslinkpool.h"

CPosLinkPool::CPosLinkPool()
{
	m_pBase       = NULL;
	m_PosLinkList = NULL;
	m_nPoolCount  = 0;
	m_nCurrIndex  = 0;
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

size_t CPosLinkPool::Init(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase     = pData;
	size_t nPos = 0;
	
	//printf("[CAreaInfoPool::Init]nPos=%d.\n", nPos);
	m_PosLinkList = (_PosLink_Info* )&pData[nPos];
	nPos += sizeof(_PosLink_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_PosLinkList[i].Init();
		m_PosLinkList[i].Set_Index(i);
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;		
}

size_t CPosLinkPool::Load(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase       = pData;
	size_t nPos   = 0;
	
	//printf("[CNodePool::Load]nPos=%d.\n", nPos);
	m_PosLinkList = (_PosLink_Info* )&pData[nPos];
	nPos += sizeof(_PosLink_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_PosLinkList[i].m_pNext = NULL;
	}	
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;			
}

_PosLink_Info* CPosLinkPool::Get(int nIndex)
{
	if(nIndex < 0 || nIndex >= m_nPoolCount)
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
	
	if(m_nCurrIndex >= m_nPoolCount - 1)
	{
		m_nCurrIndex = 0;
	}	
	
	if(m_PosLinkList[m_nCurrIndex].m_cUsed == 0)
	{
		//printf("[CPosLinkPool::Create]m_nCurrIndex=%d, nIndex=%d.\n", m_nCurrIndex, m_PosLinkList[m_nCurrIndex].Get_Index());
		m_PosLinkList[m_nCurrIndex].m_cUsed = 1;
		return &m_PosLinkList[m_nCurrIndex++];
	}
	else
	{
		//循环寻找空位
		for(int i = m_nCurrIndex + 1; i < m_nPoolCount; i++)
		{
			if(m_PosLinkList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				if(m_nCurrIndex > m_nPoolCount - 1)
				{
					m_nCurrIndex = 0;
				}
				m_PosLinkList[i].m_cUsed = 1;
				//printf("[CPosLinkPool::Create]m_nCurrIndex=%d, nIndex=%d.\n", m_nCurrIndex, m_PosLinkList[m_nCurrIndex].Get_Index());
				return &m_PosLinkList[i];				
			}
		}
		
		//printf("[CPosLinkPool::Create]m_nCurrIndex=%d,m_nPoolCount=%d.\n", m_nCurrIndex, m_nPoolCount);
		int nStart = 0;
		//没找到，再重头开始找
		for(int i = nStart; i < m_nCurrIndex - 1; i++)
		{
			if(m_PosLinkList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				m_PosLinkList[i].m_cUsed = 1;
				//printf("[CPosLinkPool::Create]m_nCurrIndex=%d, nIndex=%d.\n", m_nCurrIndex, m_PosLinkList[m_nCurrIndex].Get_Index());
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
	
	if(pPosLinkInfo->Get_Index() >= m_nPoolCount || pPosLinkInfo->Get_Index() < 0)
	{
		printf("[CAreaInfoPool::Delete]Get_Index=%d is unvalid.\n", pPosLinkInfo->Get_Index());
		return false;
	}
	
	m_PosLinkList[pPosLinkInfo->Get_Index()].Clear();
	return true;		
}
