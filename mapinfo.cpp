#include "mapinfo.h"

CMapInfo::CMapInfo()
{
	m_AreaCount   = 0;
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
								  + m_AreaCount * sizeof(_Pos_Info)
								  + m_AreaCount * sizeof(_Hash_Table_Cell)* 2;
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
	m_objAreaInfoList.Init(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_Area_Info);
	
	//初始化用户内存池
	m_objPosInfoList.Init(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_Pos_Info);
	
	//初始化区域Hash表
	m_objHashArea.Init((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);	
	
	//初始化当前点Hash表
	m_objHashCurrPos.Init((char* )(pData + nPos), m_AreaCount, m_pCryptTable);	
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);	
	
	printf("[CMapInfo::Init]nPos=%d.\n", nPos);
}

void CMapInfo::Load(char* pData)
{
	size_t nPos = 0;
	m_pCryptTable = pData;
	nPos += 1280;		
	
	//加载区域内存池
	m_objAreaInfoList.Load(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_Area_Info);	
	
	//加载定位点内存池
	m_objPosInfoList.Load(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_Pos_Info);	
	
	//加载区域Hash表
	m_objHashArea.Load((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);		
	
	//加载定位点Hash表
	m_objHashCurrPos.Load((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);			
}

bool CMapInfo::AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos)
{
	CGeoHash m_GeoHash;
	char pBeforeGeo[10] = {'\0'};
	char pCurrGeo[10]   = {'\0'};
	
	//首先去Hashmap里面去查是否已经存在
	_Pos_Info*  pBeforePosInfo  = NULL;
	_Area_Info* pBeforeAreaInfo = NULL;
	_Area_Info* pCurrAreaInfo   = NULL;
	
	int nPosBefore  = m_objHashCurrPos.Get_Hash_Box_Data(pMsisdn);
	if(nPosBefore >= 0)
	{
		pBeforePosInfo = m_objPosInfoList.Get_NodeOffset_Ptr(nPosBefore);
		sprintf(pBeforeGeo, "%s", m_GeoHash.Encode(pBeforePosInfo->m_dPosLatitude, pBeforePosInfo->m_dPosLongitude, GEO_PERSITION));
	}
	
	sprintf(pCurrGeo, "%s", m_GeoHash.Encode(dPosLatitude, dPosLongitude, GEO_PERSITION));
	
	if(pBeforePosInfo != NULL)
	{
		//如果之前有已存在经纬度，比较之前和现在的经纬度是否在一个区域
		if(strcmp(pCurrGeo, pBeforeGeo) != 0)
		{
			//当前经纬度和之前的经纬度已经不在一个区域，则先删除之前区域的存在点
			int nAreaBefore = m_objHashArea.Get_Hash_Box_Data(pBeforeGeo);
			pBeforeAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaBefore);
			pBeforeAreaInfo->Delete(nPosBefore);
			
			pBeforePosInfo->m_dPosLatitude  = dPosLatitude;
			pBeforePosInfo->m_dPosLongitude = dPosLongitude;
			pBeforePosInfo->m_ttCurrTime    = ttPos;
			
			//查找新区域是否已存在
			int nAreaCurr = m_objHashArea.Get_Hash_Box_Data(pCurrGeo);
			if(nAreaCurr >= 0)
			{
				//如果已经存在
				pCurrAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaCurr);
				bool blState = pCurrAreaInfo->Add(nPosBefore);
				if(false == blState)
				{
					printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
					return false;				
				}
			}
			else
			{
				//如果不存在
				pCurrAreaInfo = m_objAreaInfoList.Create();
				if(NULL == pCurrAreaInfo)
				{
					printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
					return false;							
				}
				
				bool blState = pCurrAreaInfo->Add(nPosBefore);
				if(false == blState)
				{
					printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
					return false;				
				}
				
				//添加新区域
				m_objHashArea.Add_Hash_Data(pCurrGeo, m_objAreaInfoList.Get_Node_Offset(pCurrAreaInfo));				
			}	
		}
		else
		{
			//在同一区域内，则只更新当前点信息
			pBeforePosInfo->m_dPosLatitude  = dPosLatitude;
			pBeforePosInfo->m_dPosLongitude = dPosLongitude;
			pBeforePosInfo->m_ttCurrTime    = ttPos;		
		}		
	}
	else
	{
		//之前点并不存在
		pBeforePosInfo = m_objPosInfoList.Create();
		if(NULL == pBeforePosInfo)
		{
			printf("[CMapInfo::AddPos]m_objPosInfoList is empty.\n");
			return false;
		}
		
		pBeforePosInfo->m_dPosLatitude  = dPosLatitude;
		pBeforePosInfo->m_dPosLongitude = dPosLongitude;
		pBeforePosInfo->m_ttCurrTime    = ttPos;		
		
		//查找新区域是否已存在
		int nAreaCurr = m_objHashArea.Get_Hash_Box_Data(pCurrGeo);
		if(nAreaCurr >= 0)
		{
			//如果已经存在
			pCurrAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaCurr);
			bool blState = pCurrAreaInfo->Add(nPosBefore);
			if(false == blState)
			{
				printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
				return false;				
			}
		}
		else
		{
			//如果不存在
			pCurrAreaInfo = m_objAreaInfoList.Create();
			if(NULL == pCurrAreaInfo)
			{
				printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
				return false;							
			}
			
			bool blState = pCurrAreaInfo->Add(nPosBefore);
			if(false == blState)
			{
				printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
				return false;				
			}
			
			//添加新区域
			m_objHashArea.Add_Hash_Data(pCurrGeo, m_objAreaInfoList.Get_Node_Offset(pCurrAreaInfo));				
		}	
	}
	
	return true;
}
