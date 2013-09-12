//
//  main.cpp
//  myThriftApp
//
//  Created by momo@ben on 13-9-8.
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

#include "json.h"
#include "def.h"
#include "comm_func.h"

#include "onlineLogic.h"
#include "serverLogic.h"


int main(int argc, char *argv[])
{
    //声明Svr逻辑变量
    CService svr;
    int ret = 0 ;
    int clientfd;

    // //定义临时存储区
    char recbuf[255] = {0};
    char data[20] = {0};
    
    //声明在线逻辑变量
    COnlineLogic g_onlineLogic;
    key_t onlineShmid = svr.shm_create();
    ret = g_onlineLogic.initOnlineShmat(onlineShmid);
    if( 0 != ret )
    {
        printf("initOnlineShmat error %d \n", ret);
        return ret;
    }

    //绑定端口
    int svrfd = svr.BindPort(MYPORT);
    ret = svr.Listen(svrfd);
    if( 0 != ret )
    {
        printf("initOnlineShmat error %d \n", ret);
        return ret;
    }
    printf("-------------- server start ----------------\n");
    
    while(1)
    {
        //接收客户端连接
        clientfd = svr.Accept(svrfd);
        if( 0 != ret )
        {
            printf("Accept error %d svrfd %d\n", ret, svrfd);
            return ret;
        }

        //发送问候信息
        send(clientfd, WELCOME, strlen(WELCOME), 0);
        
        //创建子进程
        pid_t ppid = fork();
        if(ppid == 0)
        {
            printf("###----------- child process working -------------###\n");
            
            while(1)
            {
                printf("######-------- cmd process start ---------######\n");
                int recvbytes = 0;
                memset(recbuf,0,255);
                
                //客户端主动关闭
                if((recvbytes = recv(clientfd,recbuf,255,0)) <= 0)
                {
                    printf("######clientfd %d recv error \n", clientfd);
                    
                    g_onlineLogic.updateOnlineStat(clientfd, ONLINE_STAT_OFF);
                    g_onlineLogic.displayOnlineShmat();
                    
                    close(clientfd);
                    raise(SIGKILL);
                    exit(1);
                }
                
                //打印收包信息
                CCommFunc::get_cur_time(data);
                printf("######Data:%s\n",data);
                printf("######Revice Buffer:%s\n",recbuf);
            
                //解析请求命令字与请求包体
                std::string cmd, body;
                ret = CCommFunc::splitRequest( recbuf, cmd, body);
                if( 0 != ret )
                {
                    printf("######CCommFunc::splitRequest revbuffer %s ret %d\n", recbuf, ret);
                    continue;
                }
                printf("######Cmd:%s Body:%s\n", cmd.c_str(), body.c_str());
                
                //---------------------业务逻辑-----------------------
                //用户注册
                if( 0 == strcmp(cmd.c_str(), "reg") )
                {
                    g_onlineLogic.regOnlineShmat(body.c_str(), clientfd);
                    g_onlineLogic.displayOnlineShmat();
                }
                //获取在线列表
                else if( 0 == strcmp(cmd.c_str(), "onlineinfo") )
                {
                    std::string list;
                    g_onlineLogic.getOnlineList(list);
                    
                    //send temp buffer
                    svr.sendResponse(clientfd,cmd,list);
                }
                //选择对手
                else if( 0 == strcmp(cmd.c_str(), "vs") )
                {
                    g_onlineLogic.vs(clientfd, body);
                    g_onlineLogic.displayOnlineShmat();
                }
                //攻击对手
                else if( 0 == strcmp(cmd.c_str(), "hit") )
                {
                    int vsfd = g_onlineLogic.getVsFD(clientfd);
                    printf("######clientfd |%d| vsfd |%d|\n", clientfd, vsfd);
                    if( 0 != vsfd )
                    {
                        //send temp buffer
                        svr.sendResponse(vsfd,cmd,body);
                    }
                }
                else
                {
                    printf("######can not found func for %s \n", cmd.c_str());
                }
                //---------------------业务逻辑-----------------------
                printf("######-------- cmd process end ---------######\n\n\n");
                
            }
            printf("###----------- child process stoped -------------###\n");
        }
    }
    printf("-------------- server stop ----------------\n");

    close(svrfd);
    close(clientfd);
    return 0;
}

