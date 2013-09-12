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
#include    <limits.h>        /* for OPEN_MAX */

#include "poll.h"
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
    
    int nready;
    struct pollfd client[OPEN_MAX];
    
    
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
    
    //init client
    int maxi = 0, i = 0;
    client[0].fd = svrfd;
    client[0].events = POLLRDNORM;
    for( i = 1 ; i < OPEN_MAX; i++ )
    {
        client[i].fd = -1;
    }

    
    printf("-------------- server start ----------------\n");
    for(;;)
    {
        //poll begin
        nready = poll(client, maxi+1, INFTIM);
        
        //new client connection
        if( client[0].revents & POLLRDNORM )
        {
            printf("new connection \n");
            sockaddr_in cliaddr;
            socklen_t clilen = sizeof(cliaddr);
            
            int connfd = accept(svrfd, (struct sockaddr *)&cliaddr, &clilen);
         
            //save connection discriptor
            for( i = 1 ; i < OPEN_MAX; i++ )
            {
                if( client[i].fd < 0 )
                {
                    client[i].fd = connfd;
                    
                    //发送问候信息
                    send(connfd
                         
                         , WELCOME, strlen(WELCOME), 0);
                    break;
                }
            }
            
            if( i == OPEN_MAX )
            {
                printf("###too many clients\n");
                continue;
            }
            
            client[i].events = POLLRDNORM;
            
            //count max index in client array
            printf("i %d maxi %d\n", i, maxi);
            if( i > maxi )
            {
                maxi = i;
            }
            
            //no more reader discriptor
            if( --nready <= 0 )
            {
                continue;
            }
        }
        
        for( i = 1 ; i <= maxi ; i++ )
        {
            //check client fd
            if( (clientfd = client[i].fd) < 0 )
            {
                printf("i %d fd %d \n", i, client[i].fd);
                continue;
            }
            
            printf("i %d clientfd %d send request\n", i, clientfd);
            if( client[i].revents &(POLLRDNORM | POLLERR) )
            {
                //recive request data
                memset(recbuf, 0, sizeof(recbuf));
                if( (ret = read(clientfd, recbuf, MAXLINE)) < 0 )
                {
                    //connection reset by client
                    if( errno == ECONNRESET )
                    {
                        close(clientfd);
                        client[i].fd = -1;
                        printf("###read error ECONNERSET\n");
                    }
                    else
                    {
                        printf("###read error %d\n", ret);
                    }
                }
                //connection closed by client
                else if( ret == 0 )
                {
                    close(clientfd);
                    client[i].fd = -1;
                    printf("###read finished!\n");
                }
                else
                {
                    printf("######-------- cmd process start ---------######\n");
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
            }
        }
    }
    printf("-------------- server stop ----------------\n");

    close(svrfd);
    close(clientfd);
    return 0;
}

