#include "mapinfo.h"
#include "ShareMemory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include "soapH.h"
#include "httppost.h"
#include "json.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" 

using namespace rapidjson;

#define BACKLOG (100)
#define MAX_THR (10)
#define MAX_QUEUE (1000)

#define HTTP_PORT 10000

#define JSON_FINDPOS_TEXT "\"Msisdn\":\"%s\",\"Latitude\":\"%f\",\"Longitude\":\"%f\",\"CurrTime\":\"%ld\""

SOAP_SOCKET queue[MAX_QUEUE];

//全局函数
CMapInfo g_objMapInfo;

int head = 0;
int tail = 0;
pthread_mutex_t queue_cs;
pthread_cond_t queue_cv;

static SOAP_SOCKET dequeue();
static int enqueue(SOAP_SOCKET sock);
static void *process_queue(void *soap);
int text_post_handler(struct soap *soap);
int sendPost(char *url,char *cont);

SOAP_NMAC struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{NULL, NULL, NULL, NULL}
};

int __ns1__add(struct soap *soap, struct ns2__pair *in, double *out)
{ *out = in->a + in->b;
  return SOAP_OK;
}

int __ns1__sub(struct soap *soap, struct ns2__pair *in, double *out)
{ *out = in->a - in->b;
  return SOAP_OK;
}

int __ns1__mul(struct soap *soap, struct ns2__pair *in, double *out)
{ *out = in->a * in->b;
  return SOAP_OK;
}

int __ns1__div(struct soap *soap, struct ns2__pair *in, double *out)
{ *out = in->a / in->b;
  return SOAP_OK;
}

int __ns1__pow(struct soap *soap, struct ns2__pair *in, double *out)
{ *out = pow(in->a, in->b);
  return SOAP_OK;
}

static int enqueue(SOAP_SOCKET sock)
{
    int status = SOAP_OK;
    int next;
    pthread_mutex_lock(&queue_cs);
    next = tail + 1;
    if (next >= MAX_QUEUE)
        next = 0;
    if (next == head)
        status = SOAP_EOM;
    else
    {
        queue[tail] = sock;
        tail = next;
    }
    pthread_cond_signal(&queue_cv);
    pthread_mutex_unlock(&queue_cs);
    return status;
}

static SOAP_SOCKET dequeue()
{
    SOAP_SOCKET sock;
    pthread_mutex_lock(&queue_cs);
    while (head == tail)
        pthread_cond_wait(&queue_cv, &queue_cs);
    sock = queue[head++];
    if (head >= MAX_QUEUE)
        head = 0;
    pthread_mutex_unlock(&queue_cs);
    return sock;
}

static void *process_queue(void *soap)
{
    struct soap *tsoap = (struct soap*) soap;
    for (;;)
    {
        tsoap->socket = dequeue();
        if (!soap_valid_socket(tsoap->socket))
            break;
        soap_serve(tsoap);
        soap_destroy(tsoap);
        soap_end(tsoap);
    }
    return NULL;
}

int text_post_handler(struct soap *soap)
{ 
	//处理Json字符串
	Document document;	
	
	char *buf;
	char retBuf[2048] = {'\0'};
	memset(retBuf,0,sizeof(retBuf));
  size_t len;
  soap_http_body(soap, &buf, &len);
  printf("[text_post_handler]buf=%s.\n", buf);
  printf("[text_post_handler]path=%s.\n", soap->path);
	
	//解析Json, 获得查询相关参数
	if(false == document.Parse(buf).HasParseError())
	{
		double dLatitude  = 0.0f;
		double dLongitude = 0.0f;
		double dRadius    = 0.0f;		
		if(strcmp(soap->path, "/GeoHash/Search/") == 0)
		{
			//解析GeoHash查询功能参数
			if(document.HasMember("Latitude") ==  true)
			{
				dLatitude = atof(document["Latitude"].GetString());
			}
			if(document.HasMember("Longitude") ==  true)
			{
				dLongitude = atof(document["Longitude"].GetString());
			}
			if(document.HasMember("Radius") ==  true)
			{
				dRadius = atof(document["Radius"].GetString());
			}
			
			if(dLatitude != 0.0f && dLongitude != 0.0f && dRadius != 0.0f)
			{
				//这里添加geo调用算法
				vector<_Pos_Info*> vecPosList;
			 	bool blState = g_objMapInfo.FindPos(dLatitude, dLongitude, dRadius, vecPosList);
			 	
			 	if(true == blState)
			 	{
			 		//将结果拼装成Json
			 		char szTemp[500] = {'\0'};
			 		retBuf[0] = '{';
			 		int nPos  = 1;
			 		for(int i = 0; i < (int)vecPosList.size(); i++)
			 		{
			 			if(i != (int)vecPosList.size() - 1)
			 			{
			 				sprintf(szTemp, JSON_FINDPOS_TEXT, vecPosList[i]->m_szMsisdn,
 																								 vecPosList[i]->m_dPosLatitude,
 																								 vecPosList[i]->m_dPosLongitude,
 																								 vecPosList[i]->m_ttCurrTime);
 							int nLen = (int)strlen(szTemp);
 						  memcpy(&retBuf[nPos], szTemp, nLen);
 						  nPos += nLen;
 						  retBuf[nPos] = ',';
 						  nPos += 1;
 						}
 						else
 						{
			 				sprintf(szTemp, JSON_FINDPOS_TEXT, vecPosList[i]->m_szMsisdn,
 																								 vecPosList[i]->m_dPosLatitude,
 																								 vecPosList[i]->m_dPosLongitude,
 																								 vecPosList[i]->m_ttCurrTime);
 							int nLen = (int)strlen(szTemp);
 						  memcpy(&retBuf[nPos], szTemp, nLen);
 						  nPos += nLen;
 						  retBuf[nPos] = '}';
 						  nPos += 1; 							
 						}
			 		}
				}
				else
				{
					sprintf(retBuf, "{\"error\":\"2\"}");
				}
			}
			else
			{
				sprintf(retBuf, "{\"error\":\"1\"}");
			}				
		}
		else if(strcmp(soap->path, "/GeoHash/Add/") == 0)
		{
			char   szMsisdn[15]   = {'\0'};
			double dLatitude      = 0.0f;
			double dLongitude     = 0.0f;
			time_t ttNow          = 0;
						
			//解析添加当前点数据参数
			if(document.HasMember("Msisdn") ==  true)
			{
				sprintf(szMsisdn, "%s", document["Msisdn"].GetString());
			}			
			if(document.HasMember("Latitude") ==  true)
			{
				dLatitude = atof(document["Latitude"].GetString());
			}
			if(document.HasMember("Longitude") ==  true)
			{
				dLongitude = atof(document["Longitude"].GetString());
			}
			if(document.HasMember("Time") ==  true)
			{
				ttNow = (time_t)atol(document["Time"].GetString());
			}
			
			if(strlen(szMsisdn) > 0 && dLatitude != 0.0f && dLongitude != 0.0f && ttNow != 0)
			{
				bool blState = g_objMapInfo.AddPos(szMsisdn, dLatitude, dLongitude, ttNow);
				if(true == blState)
				{
					sprintf(retBuf, "{\"success\":\"0\"}");
				}
				else
				{
					sprintf(retBuf, "{\"error\":\"2\"}");
				}
			}
			else
			{
				sprintf(retBuf, "{\"error\":\"1\"}");
			}										
		}					
	}	
	
	printf("[text_post_handler]retBuf=%s.\n", retBuf);
  len = strlen(retBuf);
  soap_response(soap, SOAP_HTML);
  soap_send_raw(soap, retBuf, len);
  soap_end_send(soap);
  return SOAP_OK;
}

int main()
{
	size_t stShareSize = g_objMapInfo.GetSize(1000000);
	printf("[main]All Size=%d.\n", stShareSize); 
	
	int nPoolSize = 600000;
	shm_key obj_key = 30010;
	shm_id obj_shm_id;
	bool blCreate = true;		
	char* pData = Open_Share_Memory_API(obj_key, stShareSize, obj_shm_id, blCreate);
	//char* pData = new char[stShareSize];
	
	if(NULL != pData)
	{
		if(blCreate == true)
		{
			memset(pData, 0, stShareSize);
			g_objMapInfo.Init(pData);
		}
		else
		{
			g_objMapInfo.Load(pData);
		}
	}
	else
	{
		printf("[main]Create share memory is fail.\n");
		return 0;
	}
	
	/*
	//测试第一个点
	time_t ttNow = time(NULL);
	objMapInfo.AddPos("13661201023", 39.928167, 116.389550, ttNow);
	
	//测试第二个点
	objMapInfo.AddPos("13661201024", 39.928367, 116.389550, ttNow);
	
	vector<_Pos_Info*> vecPosList;
 	objMapInfo.FindPos(39.928367, 116.389550, 100000.0, vecPosList);
 	for(int i = 0; i < (int)vecPosList.size(); i++)
 	{
 		printf("[FindPos]m_szMsisdn=%s, m_dPosLatitude=%f, m_dPosLongitude=%f.\n", vecPosList[i]->m_szMsisdn,
 																																							 vecPosList[i]->m_dPosLatitude,
 																																							 vecPosList[i]->m_dPosLongitude);
 	}
 	*/
 	
	int ret = 0;
	char *buf;
  size_t len;
  struct soap soap;

  soap_init(&soap);
  soap_set_omode(&soap, SOAP_C_UTFSTRING);

  struct http_post_handlers handlers[] =
  { 
    { "text/*",    text_post_handler },
    { "text/*;*",  text_post_handler },
    { "POST",      text_post_handler },
    { NULL }
  };
  soap_register_plugin_arg(&soap, http_post, handlers); 
  struct soap *soap_thr[MAX_THR];
  pthread_t tid[MAX_THR];
  SOAP_SOCKET m, s;
  int i;

  m = soap_bind(&soap, NULL, HTTP_PORT, BACKLOG);
  if (!soap_valid_socket(m))
  {
      printf("soap_valid_socket\n");
 			return 1;
  }
 
  pthread_mutex_init(&queue_cs, NULL);
  pthread_cond_init(&queue_cv, NULL);
  for (i = 0; i < MAX_THR; i++)
  {
      soap_thr[i] = soap_copy(&soap);
      soap_set_mode(soap_thr[i], SOAP_C_UTFSTRING);
      pthread_create(&tid[i], NULL, (void*(*)(void*)) process_queue,(void*) soap_thr[i]);
  }
  
  for (;;)
  {
      s = soap_accept(&soap);
      if (!soap_valid_socket(s))
      {
          if (soap.errnum)
          {
              soap_print_fault(&soap, stderr);
              continue; 
          }
          else
          {  
              break;
          }
      }
      while (enqueue(s) == SOAP_EOM)
      {
          sleep(1);
      }
  }
 
  for (i = 0; i < MAX_THR; i++)
  {
      while (enqueue(SOAP_INVALID_SOCKET) == SOAP_EOM)
      {
          sleep(1);
      }
  }
 
  for (i = 0; i < MAX_THR; i++)
  {
      pthread_join(tid[i], NULL);
      soap_done(soap_thr[i]);
      free(soap_thr[i]);
  }
  
  pthread_mutex_destroy(&queue_cs);
  pthread_cond_destroy(&queue_cv);
 
  soap_done(&soap); 	
	
	//delete[] pData;
	return 0;
}
