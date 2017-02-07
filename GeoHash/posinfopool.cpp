#include "posinfopool.h"

CPosInfoPool::CPosInfoPool()
{
	m_pBase        = NULL;
	m_PosInfoList  = NULL;
	m_nPoolCount   = 0;
	m_nCurrIndex   = 0;
}

CPosInfoPool::~CPosInfoPool()
{
	Close();
}

void CPosInfoPool::Close()
{
	m_pBase        = NULL;	
	m_PosInfoList  = NULL;
}

size_t CPosInfoPool::Init(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase     = pData;
	size_t nPos = 0;
	
	//printf("[CPosInfoPool::Init]nPos=%d.\n", nPos);
	m_PosInfoList = (_Pos_Info* )&pData[nPos];
	nPos += sizeof(_Pos_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_PosInfoList[i].Init();
		m_PosInfoList[i].Set_Index(i);
		//printf("[CPosInfoPool::Init](0)nPos=%d.\n", nPos);
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;	
}

size_t CPosInfoPool::Load(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase       = pData;
	size_t nPos   = 0;
	
	//printf("[CNodePool::Load]nPos=%d.\n", nPos);
	m_PosInfoList = (_Pos_Info* )&pData[nPos];
	nPos += sizeof(_Pos_Info) * nPoolCount;
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;		
}

_Pos_Info* CPosInfoPool::Create()
{
	if(NULL == m_PosInfoList)
	{
		return NULL;
	}
	
	if(m_nCurrIndex >= m_nPoolCount - 1)
	{
		m_nCurrIndex = 0;
	}	
	
	if(m_PosInfoList[m_nCurrIndex].m_cUsed == 0)
	{
		//printf("[CPosInfoPool::Create]m_nCurrIndex=%d, nIndex=%d.\n", m_nCurrIndex, m_PosInfoList[m_nCurrIndex].Get_Index());
		m_PosInfoList[m_nCurrIndex].m_cUsed = 1;
		return &m_PosInfoList[m_nCurrIndex++];
	}
	else
	{
		//循环寻找空位
		for(int i = m_nCurrIndex + 1; i < m_nPoolCount; i++)
		{
			if(m_PosInfoList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				if(m_nCurrIndex > m_nPoolCount - 1)
				{
					m_nCurrIndex = 0;
				}
				m_PosInfoList[i].m_cUsed = 1;
				return &m_PosInfoList[i];				
			}
		}
		
		printf("[CPosInfoPool::Create]m_nCurrIndex=%d,m_nPoolCount=%d.\n", m_nCurrIndex, m_nPoolCount);
		int nStart = 0;
		//没找到，再重头开始找
		for(int i = nStart; i < m_nCurrIndex - 1; i++)
		{
			if(m_PosInfoList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				m_PosInfoList[i].m_cUsed = 1;
				return &m_PosInfoList[i];				
			}			
		}
		
		//已经没有空位
		return NULL;
	}		
}

int CPosInfoPool::Get_Node_Offset(_Pos_Info* pWordInfo)
{
	int nOffset = 0;
	if(NULL != pWordInfo)
	{
		nOffset = (int)((char* )pWordInfo - m_pBase);
	}
	
	return nOffset;
}
	
_Pos_Info* CPosInfoPool::Get_NodeOffset_Ptr(int nOffset)
{
	return (_Pos_Info* )(m_pBase + nOffset);
}

bool CPosInfoPool::Delete(_Pos_Info* pWordInfo)
{
	if(NULL == m_PosInfoList || NULL == pWordInfo)
	{
		return false;
	}	
	
	if(-1 == pWordInfo->Get_Index())
	{
		return false;
	}
	
	if(pWordInfo->Get_Index() >= m_nPoolCount || pWordInfo->Get_Index() < 0)
	{
		printf("[CPosInfoPool::Delete]Get_Index=%d is unvalid.\n", pWordInfo->Get_Index());
		return false;
	}
	
	m_PosInfoList[pWordInfo->Get_Index()].m_cUsed = 0;
	return true;	
}
