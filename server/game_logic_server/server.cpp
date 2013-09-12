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

#include "server.h"
#include "OnlineLogic.h"
#include "comm_func.h"


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
int CService::bindPort(unsigned short int port)
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


int main(int argc, char *argv[])
{
    //声明Svr逻辑变量
    CService svr;
    
    //声明在线逻辑变量
    COnlineLogic g_onlineLogic;
    key_t onlineShmid = svr.shm_create();
    onlineShmid = g_onlineLogic.initOnlineShmat(onlineShmid);
    
    
    int clientfd;
    char *buf;

    //绑定端口
    int svrfd = svr.bindPort(MYPORT);
    
    //在指定端口上监听
    if(listen(svrfd,BACKLOG) == -1)
    {
        printf("listen error");
        exit(1);
    }
    printf("listening svrfd %d......\n", svrfd);
    
    
    // //定义临时存储区
    char *temp, *data;
    temp = (char *)malloc(255);
    data=(char *)malloc(20);
    
    while(1)
    {
        //接收客户端连接
        socklen_t sin_size = 0;
        struct sockaddr_in their_addr;
        if((clientfd = accept(svrfd,(struct sockaddr*)&their_addr,&sin_size)) == -1)
        {
            printf("accept error");
            exit(1);
        }
        printf("discriptor:%d\n",clientfd);
        
        //发送问候信息
        send(clientfd, WELCOME, strlen(WELCOME), 0);
        
        //build buff
        buf = (char *)malloc(255);
        	
        //创建子进程
        pid_t ppid = fork();
        if(ppid == 0)
        {
            printf("child process working...\n");
            
            //再次创建子进程
            //pid_t pid = fork();
            while(1)
            {
                //父进程用于接收信息
                //if(pid > 0)
                {
                    printf("grandchild process begin recv...\n");
                    memset(buf,0,255);
                    //sleep(1);
                    
                    int recvbytes = 0;
                    if((recvbytes = recv(clientfd,buf,255,0)) <= 0)
                    {
                        printf("recv1 error");
                        g_onlineLogic.updateOnlineStat(clientfd, ONLINE_STAT_OFF);
                        g_onlineLogic.displayOnlineShmat();
                        
                        close(clientfd);
                        raise(SIGKILL);
                        exit(1);
                    }
                    
                    //write buf's data to share memory
                    //memset(w_addr, '\0', 1024);
                    //strncpy(w_addr, buf, 1024);
                    //printf("w_add:%s\n", w_addr);
                    
                    //strcat time info
                    CCommFunc::get_cur_time(data);
                    //strcat(buf,data);
                    printf("buf:%s data:%s\n",buf, data);
                }
                
                //子进程用于发送信息
                //else if(pid == 0)
                {
                    //int fdStat = g_onlineLogic.getOnlineStat(clientfd);
                    //if( ONLINE_STAT_OFF == fdStat )
                    //{
                    //    printf("client fd:%d stat is %d offline!\n", clientfd, fdStat);
                    //    exit(0);
                    //}
                    
                    //sleep(1);
                    //swap shmat buffer
                    //if(strcmp(temp,r_addr) != 0)
                    {
                        //printf("grandchild process begin app logic...\n");
                        //printf("swap buffer!r_addr:|%s| temp:|%s|\n", r_addr, temp);
                        CCommFunc::get_cur_time(data);
                        //strcat(r_addr,data);
                        strcpy(temp,buf);
                        printf("temp:%s\n",temp);

                        const char *delims = { "@" };
                        char *cmd = strtok( temp, delims);
                        if( NULL == cmd )
                        {
                            printf("temp:%s parse error,no cmd!\n", temp);
                            memset(temp, '\0', 1024);
                            //memset(r_addr, '\0', 1024);
                            continue;
                        }
                        
                        
                        char* rbody = strtok( NULL, delims);
                        if( NULL == rbody )
                        {
                            printf("temp:%s parse error,no rbody!\n", temp);
                            memset(temp, '\0', 1024);
                            //memset(r_addr, '\0', 1024);
                            continue;
                        }
                        printf("cmd:%s rbody:%s\n", cmd, rbody);
                        
                        
                        if( 0 == strcmp(cmd, "reg") )
                        {
                            g_onlineLogic.regOnlineShmat(rbody, clientfd);
                            g_onlineLogic.displayOnlineShmat();
                        }
                        else if( 0 == strcmp(cmd, "onlineinfo") )
                        {
                            std::string list;
                            g_onlineLogic.getOnlineList(list);
                            
                            //send temp buffer
                            svr.sendResponse(clientfd,cmd,list);
                        }
                        else if( 0 == strcmp(cmd, "vs") )
                        {
                            g_onlineLogic.vs(clientfd, rbody);
                            g_onlineLogic.displayOnlineShmat();
                        }
                        else if( 0 == strcmp(cmd, "hit") )
                        {
                            GET_JSON_STRING(rbody, strlen(rbody));
                            int vsfd = g_onlineLogic.getVsFD(clientfd);
                            if( 0 != vsfd )
                            {
                                //send temp buffer
                                svr.sendResponse(clientfd,cmd,rbody);
                            }
                        }
                        else
                        {
                            printf("can not found func for %s \n", cmd);
                        }
                        
                        //memset(r_addr, '\0', 1024);
                        memset(temp, '\0', 255);
                    }
                }
                //else
                //{
                //    printf("fork error");
                //}
            }
        }
    }
    printf("------------------------------\n");
    free(buf);
    close(svrfd);
    close(clientfd);
    return 0;
}

