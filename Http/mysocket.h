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
 *	log.h
 *
 * Description
 *	The head file of socket functions
 *
 * Version
 *	v1.00	(2016/4/27 14:11)
 *
 * Author
 *	tengyh
 *	tengyh@si-tech.com.cn
 *
 * Modification history
 * 	2016/4/27 13:16   tengyh
 *		inition version;
 *
 *************************************************************/
#ifndef __MY_SOCKET_H__
#define __MY_SOCKET_H__
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
int initUdpSock(sockaddr_in &cli_addr,char *seraddr,int port);
int udpRcv(int cli_sockfd,sockaddr_in cli_addr,char *buffer,int len);
int udpSend(int cli_sockfd,char *sendbuffer,sockaddr_in cli_addr,int len);
#endif /*__MY_SOCKET_H__*/
