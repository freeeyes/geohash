#include "ShareMemory.h"

char* Open_Share_Memory_API(shm_key obj_key, size_t obj_shm_size, shm_id& obj_shm_id, bool& blIsCreate)
{
#ifdef WIN32
	char sz_Key[50] = {'\0'};
	sprintf_s(sz_Key, 50, "%d", obj_key);

	blIsCreate = true;

	//设置属性
	SECURITY_DESCRIPTOR sd = {0};
	SECURITY_ATTRIBUTES sa = {0};
	::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = TRUE;

	//将共享内存文件化，因为windows共享内存随着最后进程的消失而消失
	//用文件就不会有这个现象
	//首先判定文件是否存在，如果没有则创建，如果有则比较内存大小是否一致
	//如果一致则打开
	shm_id obj_local_id;   //本地文件句柄
	DWORD access      = GENERIC_READ | GENERIC_WRITE;
	DWORD shared_mode = 7;
	DWORD creation    = OPEN_ALWAYS;
	 
	LPSECURITY_ATTRIBUTES sa_local = {0};
	obj_local_id = ::CreateFileA(sz_Key, 
							   access,
							   shared_mode,
							   sa_local,
							   creation,
							   0, 
							   0);

	//得到文件大小
	DWORD dwFileSize = ::GetFileSize(obj_local_id, NULL);
	if(dwFileSize > 0 && dwFileSize != obj_shm_size)
	{
		//如果共享内存文件大小不匹配，需要删除文件重建
		::CloseHandle(obj_local_id);
		::DeleteFileA(sz_Key);
		obj_local_id = ::CreateFileA(sz_Key, 
			access,
			shared_mode,
			sa_local,
			creation,
			0, 
			0);
	}
	else if(dwFileSize > 0)
	{
		//打开共享内存
		blIsCreate = false;
	}
	

	//创建新的共享内存
	obj_shm_id = ::CreateFileMappingA(obj_local_id, &sa, PAGE_READWRITE, 0, (DWORD)obj_shm_size, (LPCSTR)sz_Key);
	if(obj_shm_id != 0)
	{
		char* pBase = (char* )MapViewOfFile(obj_shm_id, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		//FlushViewOfFile(pBase, obj_shm_size); 
		return pBase;
	}
#else
	blIsCreate = true;

	//先寻找指定的共享内存是否存在
	obj_shm_id = shmget(obj_key, 0, PERMS_IPC);
	if(obj_shm_id > 0)
	{
		//获得当前共享内存大小
		struct shmid_ds buf;
		shmctl(obj_shm_id, IPC_STAT, &buf);
		int n_Curr_Shm_Size = buf.shm_segsz;
		if(n_Curr_Shm_Size != (int)obj_shm_size)
		{
			//共享内存大小不一致，关闭共享内存，重新创建
			int nRet = shmctl(obj_shm_id, IPC_RMID, &buf);
			if(0 != nRet)
			{
				printf("[Open_Share_Memory_API](%d)(%u)delete shm fail(%d).\n", 
					n_Curr_Shm_Size, (unsigned int)obj_shm_size, errno);
				return NULL;
			}
		}
		else
		{
			blIsCreate = false;
			return (char *)shmat(obj_shm_id, (char *)0, 0);
		}
	}

	//创建新的共享内存
	obj_shm_id = shmget(obj_key, obj_shm_size, PERMS_IPC|IPC_CREAT);
	if(obj_shm_id >= 0)
	{
		return (char *)shmat(obj_shm_id, (char *)0, 0);
	}
#endif
	return NULL;
}

void Close_Share_Memory_API(const char* pBase, shm_id& obj_shm_id, size_t stPoolSize)
{
#ifdef WIN32
	FlushViewOfFile(pBase, stPoolSize); 
	UnmapViewOfFile(pBase);
	CloseHandle(obj_shm_id); 
#else
	if(NULL != pBase)
	{
		printf("[Close_Share_Memory_API]Close obj_shm_id=%d, size=%u.\n", obj_shm_id, (unsigned int)stPoolSize);
	}
#endif
}

void Delete_Share_Memory_API(shm_key obj_key)
{
	shm_id obj_shm_id;
	char* buf;
#ifdef WIN32
	//windows暂不实现
#else	
	//先寻找指定的共享内存是否存在
	obj_shm_id = shmget(obj_key, 0, PERMS_IPC);
	if(obj_shm_id > 0)
	{
		struct shmid_ds buf;
		//共享内存大小不一致，关闭共享内存，重新创建
		int nRet = shmctl(obj_shm_id, IPC_RMID, &buf);
		if(0 != nRet)
		{
			printf("[Delete_Share_Memory_API]err=%d", errno);
		}		
	}
#endif	
}

