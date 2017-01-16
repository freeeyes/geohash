#include "mapinfo.h"

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
	m_PosInfoList = (_POS_INFO* )&pData[nPos];
	nPos += sizeof(_POS_INFO) * nPoolCount;
	
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
	m_PosInfoList = (_POS_INFO* )&pData[nPos];
	nPos += sizeof(_POS_INFO) * nPoolCount;
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;		
}

_POS_INFO* CPosInfoPool::Create()
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

int CPosInfoPool::Get_Node_Offset(_POS_INFO* pWordInfo)
{
	int nOffset = 0;
	if(NULL != pWordInfo)
	{
		nOffset = (int)((char* )pWordInfo - m_pBase);
	}
	
	return nOffset;
}
	
_POS_INFO* CPosInfoPool::Get_NodeOffset_Ptr(int nOffset)
{
	return (_POS_INFO* )(m_pBase + nOffset);
}

bool CPosInfoPool::Delete(_POS_INFO* pWordInfo)
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

//=============================================

CMapInfo::CMapInfo()
{
	m_AreaCount   = 0;
	m_pAreaInfo   = NULL;
	m_pCryptTable = NULL;
}

CMapInfo::~CMapInfo()
{
}

//hash算法对应的加密字符串词单
void CMapInfo::prepareCryptTable()
{
  unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;
  for(index1 = 0; index1 < 0x100; index1++)
  { 
    for(index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
    { 
      unsigned long temp1, temp2;
      seed = (seed * 125 + 3) % 0x2AAAAB;
      temp1 = (seed & 0xFFFF) << 0x10;
      seed = (seed * 125 + 3) % 0x2AAAAB;
      temp2 = (seed & 0xFFFF);
      if(index2 > 1280)
      {
      	printf("[prepareCryptTable]index2=%u.\n", (unsigned int)index2);
      }
      m_pCryptTable[index2] = (char)(temp1 | temp2); 
    } 
  } 		
}

size_t CMapInfo::GetSize(int nMaxCount)
{
	m_AreaCount = nMaxCount;
	
	//printf("[CMapInfo::GetSize]nCount=%d.\n", nMaxCount);
	
	size_t stSize = 1280 + sizeof(_Area_Info) * m_AreaCount
								  + m_AreaCount * sizeof(_Hash_Table_Cell)
								  + m_AreaCount * sizeof(_POS_INFO);
	return stSize;
}

void CMapInfo::Init(char* pData)
{
	size_t nPos = 0;
	m_pCryptTable = pData;
	memset(m_pCryptTable, 0, 1280);
	prepareCryptTable();
	nPos += 1280;	
	
	//初始化地图
	m_pAreaInfo = (_Area_Info* )(pData + nPos);
	nPos += sizeof(_Area_Info) * m_AreaCount;
	
	for(int i = 0; i < m_AreaCount; i++)
	{
		m_pAreaInfo[i].Init();
	}
	
	//初始化用户内存池
	m_objPosInfoList.Init(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_POS_INFO);
	
	//初始化Hash表
	m_objHashArea.Init((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);	
	printf("[CMapInfo::Init]nPos=%d.\n", nPos);
}

void CMapInfo::Load(char* pData)
{
	size_t nPos = 0;
	m_pCryptTable = pData;
	nPos += 1280;		
	
	//初始化地图
	m_pAreaInfo = (_Area_Info* )(pData + nPos);
	nPos += sizeof(_Area_Info) * m_AreaCount;	
	
	//初始化用户内存池
	m_objPosInfoList.Init(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_POS_INFO);	
	
	//初始化Hash表
	m_objHashArea.Load((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);		
}

bool CMapInfo::AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos)
{
	//首先去Hashmap里面去查是否已经存在
	
	return true;
}
