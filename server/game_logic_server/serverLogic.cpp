//
//  serverLogic.cpp
//  myThriftApp
//
//  Created by momo@ben on 13-9-12.
//
//


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  //数据类型定义
#include <sys/stat.h>
#include <netinet/in.h>  //定义数据结构sockaddr_in
#include <sys/socket.h>  //提供socket函数及数据结构
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h>
#include <time.h>
#include <string>
#include <string.h>

#include "def.h"
//#include "json.h"
#include "serverLogic.h"


CService::CService()
{
    
}

CService::~CService()
{
    
}


//创建共享存储区
key_t CService::shm_create()
{
    key_t shmid;
    //shmid = shmget(IPC_PRIVATE,1024,PERM);
    if((shmid = shmget(IPC_PRIVATE,1024,PERM)) == -1)
    {
        fprintf(stderr,"Create Share Memory Error:%s\n\a",strerror(errno));
        exit(1);
    }
    return shmid;
}


//端口绑定函数,创建套接字，并绑定到指定端口
int CService::BindPort(unsigned short int port)
{
    int sockfd;
    struct sockaddr_in my_addr;
    sockfd = socket(AF_INET,SOCK_STREAM,0);//创建基于流套接字
    my_addr.sin_family = AF_INET;//IPv4协议族
    my_addr.sin_port = htons(port);//端口转换
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),0);
    
    if(bind(sockfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr)) == -1)
    {
        printf("bind");
        exit(1);
    }
    
    printf("bing fd:%d success!\n", sockfd);
    return sockfd;
}


int CService::Listen(int fd)
{
    if(listen(fd,BACKLOG) == -1)
    {
        printf("listen error");
        return -1;
    }
    
    printf("listening svrfd %d......\n", fd);
    return 0;
}


int CService::Accept(int fd)
{
    int clientfd = 0;
    socklen_t sin_size = 0;
    struct sockaddr_in their_addr;
    
    if((clientfd = accept(fd,(struct sockaddr*)&their_addr,&sin_size)) == -1)
    {
        printf("accept error");
        return -1;
    }
    printf("discriptor:%d\n",clientfd);
    
    return clientfd;
}


int CService::sendResponse(int fd, std::string cmd, std::string rps)
{
    std::string response = cmd + "@" + rps;
    printf("sendResponse fd:%d response:%s\n", fd, response.c_str());
    if(send(fd,response.c_str(),response.length(),0) == -1)
    {
        printf("send error");
        return -1;
    }
    
    return 0;
}
