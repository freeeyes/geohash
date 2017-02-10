#include "../GeoHash/imapinfo.h"

//add by freeeyes
//这个插件的作用是，当第一次建立共享内存的时候，
//会加载来自其他数据源中的数据，因为数据源可能来自于很多方向(数据库,缓冲,文件等)。
//所以将这部分独立出来做成插件，主程序运行的时候会自动加载这个插件过程，填充初始化数据。
//由开发人员自己编写指定的代码

extern "C"
{
	int LoadModuleData(IMapInfo* pMapInfo);
}

int LoadModuleData(IMapInfo* pMapInfo)
{
	//添加你的初始化代码在这里
	printf("[LoadModuleData]Hello.\n");
	
	//测试添加两条实时点数据
	time_t ttNow = time(NULL);
	pMapInfo->AddPos("13661201024", 39.928167, 116.389550, ttNow);
	
	pMapInfo->AddPos("13661201025", 39.928167, 116.389550, ttNow);
	
	return 0;
}
