#ifndef _IMAP_INFO
#define _IMAP_INFO

#include "posinfopool.h"
#include <vector>
#include <string>
using namespace std;

//add by freeeyes
//用于提供模块的虚接口

class IMapInfo
{
public:
	IMapInfo() {};
	virtual ~IMapInfo() {};
	
	virtual bool AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos) = 0;
	virtual bool FindPos(double dPosLatitude, double dPosLongitude, double dDistance, vector<_Pos_Info*>& vecPosList) = 0;
	virtual bool DelPos(const char* pMsisdn) = 0;
	virtual string GetPoolState() = 0;
};

#endif
