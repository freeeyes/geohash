#include "areainfopool.h"

CAreaInfoPool::CAreaInfoPool()
{
	m_pBase        = NULL;
	m_AreaInfoList = NULL;
	m_nPoolCount   = 0;
	m_nCurrIndex   = 0;
}

CAreaInfoPool::~CAreaInfoPool()
{
	Close();
}

void CAreaInfoPool::Close()
{
	m_pBase        = NULL;	
	m_AreaInfoList  = NULL;
}

size_t CAreaInfoPool::Init(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase     = pData;
	size_t nPos = 0;
	
	//printf("[CAreaInfoPool::Init]nPos=%d.\n", nPos);
	m_AreaInfoList = (_Area_Info* )&pData[nPos];
	nPos += sizeof(_Area_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_AreaInfoList[i].Init();
		m_AreaInfoList[i].Set_Index(i);
		//printf("[CAreaInfoPool::Init](0)nPos=%d.\n", nPos);
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;	
}

size_t CAreaInfoPool::Load(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase       = pData;
	size_t nPos   = 0;
	
	//printf("[CNodePool::Load]nPos=%d.\n", nPos);
	m_AreaInfoList = (_Area_Info* )&pData[nPos];
	nPos += sizeof(_Area_Info) * nPoolCount;
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;		
}

_Area_Info* CAreaInfoPool::Create()
{
	if(NULL == m_AreaInfoList)
	{
		return NULL;
	}
	
	if(m_nCurrIndex >= m_nPoolCount - 1)
	{
		m_nCurrIndex = 0;
	}	
	
	if(m_AreaInfoList[m_nCurrIndex].m_cUsed == 0)
	{
		//printf("[CAreaInfoPool::Create]m_nCurrIndex=%d, nIndex=%d.\n", m_nCurrIndex, m_AreaInfoList[m_nCurrIndex].Get_Index());
		m_AreaInfoList[m_nCurrIndex].m_cUsed = 1;
		return &m_AreaInfoList[m_nCurrIndex++];
	}
	else
	{
		//循环寻找空位
		for(int i = m_nCurrIndex + 1; i < m_nPoolCount; i++)
		{
			if(m_AreaInfoList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				if(m_nCurrIndex > m_nPoolCount - 1)
				{
					m_nCurrIndex = 0;
				}
				m_AreaInfoList[i].m_cUsed = 1;
				return &m_AreaInfoList[i];				
			}
		}
		
		printf("[CAreaInfoPool::Create]m_nCurrIndex=%d,m_nPoolCount=%d.\n", m_nCurrIndex, m_nPoolCount);
		int nStart = 0;
		//没找到，再重头开始找
		for(int i = nStart; i < m_nCurrIndex - 1; i++)
		{
			if(m_AreaInfoList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				m_AreaInfoList[i].m_cUsed = 1;
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
	
	if(pWordInfo->Get_Index() >= m_nPoolCount || pWordInfo->Get_Index() < 0)
	{
		printf("[CAreaInfoPool::Delete]Get_Index=%d is unvalid.\n", pWordInfo->Get_Index());
		return false;
	}
	
	m_AreaInfoList[pWordInfo->Get_Index()].m_cUsed = 0;
	return true;	
}

