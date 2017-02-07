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
 *	mysocket.c
 *
 * Description
 *	a set of functions relate to socket
 *
 * Version
 *	v1.00	(2016/4/27 14:06)
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

#include "mysocket.h"


/*******************************************************
 *	write_available
 *		采集资源情况的数据
 *
          *	Parameters
          *		rsp	  (IN)  	资源使用数据的应答包
          *
          *	Return value
          *		0<		采集函数调用失败
          *		=0		函数调用成功
          *
 *******************************************************/
int write_available(int fd, int wait_sec)
{
    fd_set rf;
    struct timeval to;
    int ret;
    //div_t waits;
		if(fd < 0)
		{
			
			printf("read_available errno fd is**** %d\n",fd);
      //LogPrt(0,"");
      return -1; 	/* some error */	
		}
    FD_ZERO(&rf);
    FD_SET(fd, &rf);
    //waits = div(wait_usec, 1000000);
    to.tv_sec = wait_sec,//to.tv_sec = waits.quot;
    to.tv_usec = 0;//waits.rem;
    errno=0;
		
    ret = select(fd + 1, NULL,&rf, NULL, &to);
    if (ret > 0 && FD_ISSET(fd, &rf))
        return 1;
    if (ret < 0) {
        /* In most select() implementations, to will now contain the
         * remaining time rather than the original time.  That is exactly
         * what we want when retrying after an interrupt. */
				printf("write_available errno is**** %d,strerror(errno)**** is %s\n",errno,strerror(errno));
        //LogPrt(0,"");
        return -1; 	/* some error */
    }
    return 0;
}

/*******************************************************
 *	initUdpSock
 *		return int
 *
 *	Parameters
 *		
 *		
 *   
 *	Return value
 *	>=0 	socket 
 *	<0 fail	
 *		
 *
 *******************************************************/ 
int initUdpSock(sockaddr_in &cli_addr,char *seraddr,int port)
{
	socklen_t addrlen;
	int cli_sockfd = socket(AF_INET,SOCK_DGRAM,0);
  if(cli_sockfd<0)
  {
        printf("create socket error\n");
        return -1;
  }
  else
  {
  	/* 填写sockaddr_in*/
    addrlen=sizeof(struct sockaddr_in);
  	bzero(&cli_addr,addrlen);
   	cli_addr.sin_family=AF_INET;
   	cli_addr.sin_addr.s_addr=inet_addr(seraddr);
    //cli_addr.sin_addr.s_addr=htonl(INADDR_ANY);

   	cli_addr.sin_port=htons(port); 
  	return cli_sockfd;
  }
}
/*******************************************************
 *	udpSend
 *		return int
 *
 *	Parameters
 *	cli_sockfd	
 *	sendbuffer	
 *  seraddr 
 *  port
 *	Return value
 *	>0 	succcess 
 *	<=0 fail	
 *		
 *
 *******************************************************/ 
int udpSend(int cli_sockfd,char *sendbuffer,sockaddr_in cli_addr,int len)
{
 	//struct sockaddr_in cli_addr;
 	socklen_t addrlen;
 	if((cli_sockfd < 0)||(!sendbuffer))
 	{
 		return -1;	
 	}
 	printf("\nudpSend sendbuffer:");
 	for(int i = 0;i < len ;i++)
 	{
 		printf(" %x",(unsigned char)sendbuffer[i]);
 	}
  addrlen=sizeof(struct sockaddr_in);
  struct timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec =0;
	setsockopt(cli_sockfd, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, sizeof(tv));
  printf("\nudpSend socket  available");
 	ssize_t ret = sendto(cli_sockfd,sendbuffer,len,0,(struct sockaddr*)&cli_addr,addrlen);
  if(ret <= 0)
  {
    	
    	printf("\nsendto error %d %s", errno, strerror(errno));
    	return -1;
  }
  printf("\nsendto: ok ret = %d",ret);
  return ret;
}
 /*******************************************************
 *	udpRcv
 *		return int
 *
 *	Parameters
 *		
 *		
 *   
 *	Return value
 *	>=0 	len 
 *	<0 fail	
 *		
 *
 *******************************************************/ 
int udpRcv(int cli_sockfd,sockaddr_in cli_addr,char *buffer,int len)
{
	socklen_t addrlen;
	struct sockaddr  serve_addr;
	
	addrlen=sizeof(struct sockaddr_in);
	bzero(&serve_addr,addrlen);
	/* 接收server端返回的字符串*/
  int rcvLen = 0;
  struct timeval tv_out;
	tv_out.tv_sec = 3;
	tv_out.tv_usec = 0; 
	setsockopt(cli_sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv_out,sizeof(tv_out));
  do
  {
  	rcvLen = recvfrom(cli_sockfd,buffer,len,0,(struct sockaddr*)&serve_addr,&addrlen);
   //printf("receive from %s\n",inet_ntoa(cli_addr.sin_addr));
		if((rcvLen > 0) && (0 == strcmp((char *)&cli_addr,(char *)&serve_addr)))
		{
			printf("\nreceive:");
			for(int i=0;i<rcvLen;i++)
    		printf("	%x",(unsigned char)buffer[i]);
  		printf("\n");
  		break;
  	}
  	memset(buffer,0,len);
  }while(rcvLen > 0);
  printf("\nreceive: len=%d",len);
	return len;
}
