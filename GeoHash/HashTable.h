#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>

enum EM_HASH_STATE
{
	EM_SELECT = 0,
	EM_INSERT,
};

//hash表结构
struct _Hash_Table_Cell 
{
	char      m_cExists;                       //当前块是否已经使用,1已经使用，0没有被使用
	int       m_nNextKeyIndex;                 //链表信息，如果主键有冲突,记录下一个冲突主键的位置
	int       m_nProvKeyIndex;                 //链表信息，如果主键有冲突,记录上一个冲突主键的位置
	unsigned long m_uHashA;                    //第二次的hashkey值
	unsigned long m_uHashB;                    //第三次的hashkey值 
	int       m_nValue;                        //当前数据体
	
	_Hash_Table_Cell()
	{
		Init();
	}
	
	void Init()
	{
		m_cExists       = 0;
		m_uHashA        = 0;
		m_uHashB        = 0;
		m_nNextKeyIndex = -1;
		m_nProvKeyIndex = -1;
		m_nValue        = 0;
	}
	
	void Clear()
	{
		m_cExists       = 0;
		m_uHashA        = 0;
		m_uHashB        = 0;
		m_nValue        = 0;
	}  
};

//hashTable类
class CHashTable
{
public:
	CHashTable()
	{
		m_lpTable    = NULL;
		m_nCount     = 0;
		m_nUsed      = 0;
		m_cIsLocal   = 0;
		
		m_pcryptTable = NULL;
	}	
	
	~CHashTable()
	{
		Close();
	}
	
	void Init(char* pData, int nHashCount, char* pcryptTable)
	{
		m_lpTable    = NULL;
		m_nCount     = 0;
		m_nUsed      = 0;
		m_cIsLocal   = 0;
		
		m_pcryptTable = NULL;		
		
		Set_Base_Addr(pData, nHashCount, 0);	
		
		if(NULL == pcryptTable)
		{
			m_pcryptTable = new char[1280];
			memset(m_pcryptTable, 0, 1280);
			prepareCryptTable();
			m_cIsLocal    = 0;
		}
		else
		{
			m_pcryptTable = pcryptTable;
			m_cIsLocal    = 1;
		}
	}	
	
	void Load(char* pData, int nHashCount, char* pcryptTable)
	{
		m_lpTable     = NULL;
		m_pcryptTable = NULL;
		
		Set_Base_Addr(pData, nHashCount, 1);	
		
		if(NULL != pcryptTable)
		{
			m_pcryptTable = pcryptTable;
			m_cIsLocal    = 1;
		}		
	}

	void Close()
	{
		if(NULL != m_lpTable)
		{
			//delete[] m_lpTable;
			m_lpTable = NULL;
		}
		m_nCount = 0;
	}
	
	//设置一个已知的内存数组块(必须初始化调用),nInit是0重新初始化，1是不需要重新初始化
	void Set_Base_Addr(char* pData, int nCount, int nInit)
	{
		//printf("[A]nCount=%d.\n", nCount);
		m_lpTable = (_Hash_Table_Cell* )pData;
		m_nCount  = nCount;
		if(nInit == 0)
		{
			//第一次初始化
			for(int i = 0; i < m_nCount; i++)
			{
				m_lpTable[i].Init();
			}
		}
		else
		{
			//printf("[A]LOAD=%d.\n", nCount);
			//对正在使用的计数
			int nUsed = 0;
			for(int i = 0; i < m_nCount; i++)
			{
				if(m_lpTable[i].m_cExists == 1)
				{
					m_nUsed++;
				}
			}
		}
	}
	
	//得到当前缓冲块总个数
	int Get_Count()
	{
		return (int)m_nCount;
	}	

	//得到数据中正在使用的个数
	int Get_Used_Count()
	{
		return m_nUsed;
	}
	
	//得到整体数据内存大小
	size_t Get_Size()
	{
		return sizeof(_Hash_Table_Cell) * m_nCount;
	}

	//得到数组指定位置的数据
	int Get_Index(int nIndex)
	{
		if(nIndex < 0 || nIndex > m_nCount - 1)
		{
			return -1;
		}
		else
		{
			if(m_lpTable[nIndex].m_cExists == 1)
			{
				//正在使用，返回数据
				return m_lpTable[nIndex].m_nValue;
			}
			else
			{
				//数据已经无效
				return -1;
			}
		}
	}

	int Set_Index_Clear(int nIndex)
	{
		if(nIndex < 0 || nIndex > m_nCount - 1)
		{
			return -1;
		}

		if(m_lpTable[nIndex].m_cExists == 0)
		{
			return -1;
		}
		else
		{
			m_lpTable[nIndex].Clear();
			if(m_nUsed >= 1)
			{
				m_nUsed--;
			}
			return nIndex;
		}
	}

	//设置指定hash表中位置的数值
	int Set_Index(int nIndex, char* lpszString, int nValue)
	{
		const int HASH_A = 1, HASH_B = 2;
		if(nIndex < 0 || nIndex > m_nCount - 1)
		{
			return -1;
		}

		if(m_lpTable[nIndex].m_cExists == 1)
		{
			return -1;
		}
		else
		{
			m_lpTable[nIndex].m_cExists = 1;

			m_lpTable[nIndex].m_uHashA = HashString(lpszString, HASH_A);
			m_lpTable[nIndex].m_uHashB = HashString(lpszString, HASH_B);
			m_lpTable[nIndex].m_nValue = nValue;
			m_nUsed++;
			return nIndex;
		}
	}
	
	//添加一个Hash数据块
	int Add_Hash_Data(const char* pKey, int nValue)
	{
		if(NULL == m_lpTable)
		{
			//没有找到共享内存
			return -1;
		}
		
		//查找是否已经存在
		int nOffSet = Get_Hash_Box_Data(pKey);
		if(nOffSet > 0)
		{
			return 0;
		}
		
		int nPos = GetHashTablePos(pKey, EM_INSERT);
		if(-1 == nPos)
		{
			//内存已满
			return -1;
		}
		else
		{
			m_lpTable[nPos].m_nValue = nValue;
			m_nUsed++;
			return nPos;
		}		
	}
	
	//获得一个已有映射对应数值
	int Get_Hash_Box_Data(const char* pKey)
	{
		if(NULL == m_lpTable)
		{
			//没有找到共享内存
			return -1;
		}	
		
		int nPos = GetHashTablePos(pKey, EM_SELECT);
		if(-1 == nPos)
		{
			//没有找到
			return -1;
		}
		else
		{
			return m_lpTable[nPos].m_nValue;
		}			
	}
	
	//清理一个hash数据块
	int Del_Hash_Data(const char* pKey)
	{
		return DelHashTablePos(pKey);
	}
	
private:
	//hash算法对应的加密字符串词单
	void prepareCryptTable()
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
	      m_pcryptTable[index2] = (char)(temp1 | temp2); 
	    } 
	  } 		
	}
	
	//计算key对应的hash数值
	unsigned long HashString(const char* pKey, unsigned long dwHashType)
	{
		unsigned char *key = (unsigned char *)pKey;
		unsigned long seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
		int ch;
		while(*key != 0)
		{
			ch = toupper(*key++);
			seed1 = m_pcryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);
			seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
		}
		
		//printf("[HashString]seed1=%lu.\n", seed1);
		return seed1;		
	}
	
	//在已知的冲突链表中寻找最后一个
	int GetLastClashKey(int nStartIndex, const char *lpszString, unsigned long uHashA, unsigned long uHashB, EM_HASH_STATE emHashState)
	{
		int nCurrIndex = nStartIndex;
		int nMaxIndex = m_nCount;
		int nRunCount = 0;
		
		//在已知链表中寻找最后一个
		while(true)
		{
			if(nRunCount > nMaxIndex - 1)
			{
				printf("[GetLastClashKey]1 nCurrIndex=%d, nStartIndex=%d.\n", nCurrIndex, nStartIndex);
				printf("[GetLastClashKey]Get_Used_Count=%d,Count=%d.\n", Get_Used_Count(), Get_Count());
				return -1;
			}
			
			//printf("[GetLastClashKey](%s) curr(%d) next(%d)-->.\n", lpszString, nStartIndex, m_lpTable[nStartIndex].m_nNextKeyIndex);
			if(m_lpTable[nStartIndex].m_nNextKeyIndex == -1)
			{				
				//判断当前是否是当前数据
				if(uHashA == m_lpTable[nStartIndex].m_uHashA && uHashB == m_lpTable[nStartIndex].m_uHashB)
				{
					//找到了匹配的对象，返回之
					/*
					printf("[GetLastClashKey][%d](%s).\n", 
												nStartIndex, m_lpTable[nStartIndex].m_szKey);
					*/
					//printf("[GetLastClashKey]Search Count=%d.\n", nRunCount);			
					return nStartIndex;
				}			
				
				//如果是插入，就去找空余，如果不是，则直接返回
				if(emHashState == EM_INSERT)
				{
					//找到了链表末尾
					//开始寻找空余的位置
					//向后找空余
					for(int i = nStartIndex + 1; i < m_nCount; i++)
					{
						if(m_lpTable[i].m_cExists == 0)
						{
							m_lpTable[i].m_cExists = 1;
							m_lpTable[i].m_uHashA  = uHashA;
							m_lpTable[i].m_uHashB  = uHashB;
							
							//记录链表信息
							m_lpTable[nStartIndex].m_nNextKeyIndex = i;
							m_lpTable[i].m_nProvKeyIndex           = nStartIndex;
							//printf("[GetLastClashKey](%s) <--prov(%d) next(%d)-->.\n", lpszString, nStartIndex, i);
							return i;
						}
					}
					
					//向前找空余
					for(int i = 0; i <= nStartIndex - 1; i++)
					{
						//记录链表信息
						if(m_lpTable[i].m_cExists == 0)
						{
							m_lpTable[i].m_cExists = 1;
							m_lpTable[i].m_uHashA  = uHashA;
							m_lpTable[i].m_uHashB  = uHashB;

							m_lpTable[nStartIndex].m_nNextKeyIndex = i;
							m_lpTable[i].m_nProvKeyIndex           = nStartIndex;
							//printf("[GetLastClashKey](%s) <--prov(%d) next(%d)-->.\n", lpszString, nStartIndex, i);
							return i;
						}							
					}
					printf("[GetLastClashKey]2 nStartIndex=%d.\n");
				}
				
				return -1;
			}
			else
			{				
				//查看当前节点是否已经被释放
				if(emHashState == EM_INSERT)
				{
					if(m_lpTable[nStartIndex].m_cExists == 0)
					{
						m_lpTable[nStartIndex].m_cExists = 1;
						m_lpTable[nStartIndex].m_uHashA  = uHashA;
						m_lpTable[nStartIndex].m_uHashB  = uHashB;

						return nStartIndex;
					}
				}	

				//继续寻找
				if(uHashA == m_lpTable[nStartIndex].m_uHashA && uHashB == m_lpTable[nStartIndex].m_uHashB)
				{
					//找到了匹配的对象，返回之
					//printf("[GetLastClashKey]Search Count=%d.\n", nRunCount);				
					return nStartIndex;
				}
				
				/*
				printf("[GetLastClashKey][%d](%s) <--prov(%d) next(%d)-->.\n", 
								nStartIndex, m_lpTable[nStartIndex].m_szKey, 
								m_lpTable[nStartIndex].m_nProvKeyIndex, m_lpTable[nStartIndex].m_nNextKeyIndex);
				*/
				nStartIndex = m_lpTable[nStartIndex].m_nNextKeyIndex;				
			}
			
			nRunCount++;
		}		
	}
	
	//得到hash指定的位置
	int GetHashTablePos(const char *lpszString, EM_HASH_STATE emHashState)
	{
		const int HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
		unsigned long uHash = HashString(lpszString, HASH_OFFSET);
		unsigned long uHashA = HashString(lpszString, HASH_A);
		unsigned long uHashB = HashString(lpszString, HASH_B);
		unsigned long uHashStart = uHash % m_nCount, uHashPos = uHashStart;

		//printf("[GetHashTablePos]uHashPos=%d,m_nCount=%d,m_cExists=%d.\n", (int)uHashPos, m_nCount, (int)m_lpTable[uHashPos].m_cExists);
		
		if (m_lpTable[uHashPos].m_cExists == 0)
		{
			if(EM_INSERT == emHashState)
			{
				m_lpTable[uHashPos].m_cExists = 1;
				m_lpTable[uHashPos].m_uHashA  = uHashA;
				m_lpTable[uHashPos].m_uHashB  = uHashB;

				//printf("[GetHashTablePos] return uHashPos=%d 1.\n", (int)uHashPos);
				return (int)uHashPos;
			}
			else
			{
				return  GetLastClashKey((int)uHashStart, lpszString, uHashA, uHashB, emHashState);
			}
					
		}
		else if(m_lpTable[uHashPos].m_uHashA == uHashA && m_lpTable[uHashPos].m_uHashB == uHashB)	
		{
			//如果两次hash值相等，则直接返回
			//printf("[GetHashTablePos] return uHashPos=%d 2.\n", (int)uHashPos);
			return (int)uHashPos;
		}
		else
		{
			int nPos = GetLastClashKey((int)uHashStart, lpszString, uHashA, uHashB, emHashState);
			//printf("[GetHashTablePos]key=%s (%d) -> (%d)  .\n", lpszString, uHashStart, nPos);
			return nPos;
		}
		
		//printf("[GetHashTablePos]no find.\n");
		return -1; //Error value		
	}
	
	int DelHashTablePos(const char *lpszString)
	{
		int nPos = GetHashTablePos(lpszString, EM_SELECT);
		if(-1 == nPos)
		{
			 GetHashTablePos(lpszString, EM_SELECT);
			return -1;
		}
		else
		{
			if(-1 != m_lpTable[nPos].m_nProvKeyIndex)
			{
				m_lpTable[m_lpTable[nPos].m_nProvKeyIndex].m_nNextKeyIndex = m_lpTable[nPos].m_nNextKeyIndex;
			}
			
			if(-1 != m_lpTable[nPos].m_nNextKeyIndex)
			{
				m_lpTable[m_lpTable[nPos].m_nNextKeyIndex].m_nProvKeyIndex = m_lpTable[nPos].m_nProvKeyIndex;
			}			
			
			m_lpTable[nPos].Clear();
			if(m_nUsed >= 1)
			{
				m_nUsed--;
			}
			return nPos;
		}			
	}
	
private:
	_Hash_Table_Cell*    m_lpTable;	
	char*                m_pcryptTable;
	char                 m_cIsLocal;  //0内部指针，1外部指针
	int                  m_nUsed;
	int                  m_nCount;
};


#endif
