#include "posinfopool.h"

CPosInfoPool::CPosInfoPool()
{
	m_pBase        = NULL;
	m_PosInfoList  = NULL;
	m_pPoolCount   = NULL;
	m_pUsedCount   = NULL;
	m_pCurrIndex   = NULL;
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

size_t CPosInfoPool::GetSize(int nCount)
{
	return sizeof(_Pos_Info) * nCount + sizeof(int) * 3;
}

size_t CPosInfoPool::Init(int nPoolCount, char* pData)
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
	
	//printf("[CPosInfoPool::Init]nPos=%d.\n", nPos);
	m_PosInfoList = (_Pos_Info* )&pData[nPos];
	nPos += sizeof(_Pos_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_PosInfoList[i].Init();
		m_PosInfoList[i].Set_Index(i);
		//printf("[CPosInfoPool::Init](0)nPos=%d.\n", nPos);
	}
	
	(*m_pPoolCount)   = nPoolCount;
	(*m_pUsedCount)   = 0;
	(*m_pCurrIndex)   = 0;	
	
	return nPos;	
}

size_t CPosInfoPool::Load(int nPoolCount, char* pData)
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
	m_PosInfoList = (_Pos_Info* )&pData[nPos];
	nPos += sizeof(_Pos_Info) * nPoolCount;
	
	/*
	//遍历得到当前正在使用的对象个数
	for(int i = 0; i < (*m_pPoolCount); i++)
	{
		if(m_PosInfoList[i].m_cUsed == 1)
		{
			printf("[CPosInfoPool::Load]msisdn=%s.\n", m_PosInfoList[i].m_szMsisdn);
		}
	}
	*/
	
	return nPos;		
}

_Pos_Info* CPosInfoPool::Create()
{
	if(NULL == m_PosInfoList)
	{
		return NULL;
	}
	
	if((*m_pCurrIndex) >= (*m_pPoolCount) - 1)
	{
		(*m_pCurrIndex) = 0;
	}	
	
	if(m_PosInfoList[(*m_pCurrIndex)].m_cUsed == 0)
	{
		//printf("[CPosInfoPool::Create](*m_pCurrIndex)=%d, nIndex=%d.\n", (*m_pCurrIndex), m_PosInfoList[(*m_pCurrIndex)].Get_Index());
		m_PosInfoList[(*m_pCurrIndex)].m_cUsed = 1;
		(*m_pUsedCount)++;
		return &m_PosInfoList[(*m_pCurrIndex)++];
	}
	else
	{
		//循环寻找空位
		for(int i = (*m_pCurrIndex) + 1; i < (*m_pPoolCount); i++)
		{
			if(m_PosInfoList[i].m_cUsed == 0)
			{
				(*m_pCurrIndex) = i + 1;
				if((*m_pCurrIndex) > (*m_pPoolCount) - 1)
				{
					(*m_pCurrIndex) = 0;
				}
				m_PosInfoList[i].m_cUsed = 1;
				(*m_pUsedCount)++;
				return &m_PosInfoList[i];				
			}
		}
		
		printf("[CPosInfoPool::Create](*m_pCurrIndex)=%d,(*m_pPoolCount)=%d.\n", (*m_pCurrIndex), (*m_pPoolCount));
		int nStart = 0;
		//没找到，再重头开始找
		for(int i = nStart; i < (*m_pCurrIndex) - 1; i++)
		{
			if(m_PosInfoList[i].m_cUsed == 0)
			{
				(*m_pCurrIndex) = i + 1;
				m_PosInfoList[i].m_cUsed = 1;
				(*m_pUsedCount)++;
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
	
	if(pWordInfo->Get_Index() >= (*m_pPoolCount) || pWordInfo->Get_Index() < 0)
	{
		printf("[CPosInfoPool::Delete]Get_Index=%d is unvalid.\n", pWordInfo->Get_Index());
		return false;
	}
	
	m_PosInfoList[pWordInfo->Get_Index()].m_cUsed = 0;
	(*m_pUsedCount)--;
	return true;	
}

_Pool_Info CPosInfoPool::Get_Pool_State()
{
	_Pool_Info obj_Pool_Info;
	obj_Pool_Info.m_nUsedCount = (*m_pUsedCount);
	obj_Pool_Info.m_nPoolCount = (*m_pPoolCount);
	//printf("[CPosInfoPool::Get_Pool_State](*m_pUsedCount)=%d,(*m_pPoolCount)=%d.\n", (*m_pUsedCount), (*m_pPoolCount));
	return obj_Pool_Info;
}
