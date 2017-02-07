/**************************************************************
 * Copyright (C), 1995-2005, SI-TECH Information Technology Co.,Ltd.
 * All Rights Reserved.
 *
 * This SOURCE CODE FILE,which has been provided by SI-TECH as part of an 
 * SI-TECH product for use ONLY by licensed users of the product, includes
 * CONFIDENTIAL and PROPRIETARY information of SI-TECH.
 *
 **************************************************************
 * Filename
 *	msg.h
 *
 * Description
 *	The head file of message queue functions
 *
 * Version
 *	v1.00	(2016/4/27 14:11)
 *
 * Author
 *	tengyh
 *	tengyh@si-tech.com.cn
 *
 * Modification history
 * 	
 *		inition version;
 *
 *************************************************************/
#ifndef __JSON_H__
#define __JSON_H__


typedef struct Token
{
 	int type_;
  char * start_;
  char * end_;
}token_data;

typedef struct json
{
	char * begin_;
	char * end_;
	char * current_;
}json_data;

enum TokenType
{
	tokenEndOfStream = 0,
	tokenObjectBegin,
	tokenObjectEnd,
  tokenArrayBegin,
  tokenArrayEnd,
  tokenString,
  tokenNumber,
  tokenTrue,
  tokenFalse,
  tokenNull,
  tokenArraySeparator,
  tokenMemberSeparator,
  tokenComment,
  tokenError
 };
 
int jsonReadValue(char *src,char *object,char *value,json_data *pJson_data,int isBegin);
int jsonWriteBegin(char * buff,char *name,char *value);
int jsonWriteEnd(char * buff);
int jsonWriteNext(char * pBuff,char *name,char *value);
#endif /*__JSON_H__*/
