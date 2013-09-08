#include<stdio.h>   
#include<stdlib.h>
#include<sys/types.h>  //数据类型定义
#include<sys/stat.h>
#include<netinet/in.h>  //定义数据结构sockaddr_in
#include<sys/socket.h>  //提供socket函数及数据结构
#include<unistd.h>
#include<signal.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/shm.h>
#include<time.h>
#include <string>

#include "OnlineLogic.h"

using namespace std;

#define PERM S_IRUSR|S_IWUSR  
#define MYPORT 3490  //宏定义定义通信端口
#define BACKLOG 10 //宏定义，定义服务程序可以连接的最大客户数量

//宏定义，当客户端连接服务端时，想客户发送此欢迎字符串
#define WELCOME "|----------Welcome to the plane room! ----------|"


#define bool int
#define true 1
#define false 0

//转换函数，将int类型转换成char *类型
void itoa(int i,char*string)
{
    int power,j;
    j=i;
    
    for(power=1;j>=10;j/=10)
    {
        power*=10;
    }
    
    for(;power>0;power/=10)
    {
        *string++='0'+i/power;
        i%=power;
    }
    *string='\0';
}


//得到当前系统时间
void get_cur_time(char * time_str)
{
    time_t timep;
    struct tm *p_curtime;
    char *time_tmp;
    time_tmp=(char *)malloc(2);
    memset(time_tmp,0,2);

    memset(time_str,0,20);
    time(&timep);
    p_curtime = localtime(&timep);
    strcat(time_str," (");
    itoa(p_curtime->tm_hour,time_tmp);
    strcat(time_str,time_tmp);
    strcat(time_str,":");
    itoa(p_curtime->tm_min,time_tmp);
    strcat(time_str,time_tmp);
    strcat(time_str,":");
    itoa(p_curtime->tm_sec,time_tmp);
    strcat(time_str,time_tmp);
    strcat(time_str,")");
    free(time_tmp);
}


//创建共享存储区
key_t shm_create()
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
int bindPort(unsigned short int port)
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
        perror("bind");
        exit(1);
    }
    printf("bing success!\n");
    return sockfd;
}


COnlineLogic g_onlineLogic;
int main(int argc, char *argv[])
{
    int clientfd;
    char *buf;

    //绑定端口
    int sockfd = bindPort(MYPORT);
    
    //创建共享存储区
    key_t shmid, onlineShmid;
    char *r_addr, *w_addr;
    shmid = shm_create();
    w_addr = (char*)shmat(shmid, 0, 0);
    r_addr = (char*)shmat(shmid, 0, 0);
    
    //init online info
    onlineShmid = g_onlineLogic.initOnlineShmat();
    
    // //定义临时存储区
    char *temp, *time_str;
    temp = (char *)malloc(255);
    time_str=(char *)malloc(20);
    
    
    while(1)
    {
        //在指定端口上监听
        if(listen(sockfd,BACKLOG) == -1)
        {
            perror("listen error");
            exit(1);
        }
        printf("listening......\n");
        
        
        //接收客户端连接
        socklen_t sin_size = 0;
        struct sockaddr_in their_addr;
        if((clientfd = accept(sockfd,(struct sockaddr*)&their_addr,&sin_size)) == -1)
        {
            perror("accept error");
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
            printf("fork ppid=0\n");
            
            //再次创建子进程
            pid_t pid = fork();
            while(1)
            {
                //父进程用于接收信息
                if(pid > 0)
                {
                    memset(buf,0,255);
                    printf("begin recv\n");
                    //sleep(1);
                    
                    int recvbytes = 0;
                    if((recvbytes = recv(clientfd,buf,255,0)) <= 0)
                    {
                        perror("recv1 error");
                        close(clientfd);
                        raise(SIGKILL);
                        exit(1);
                    }
                    
                    
                    //write buf's data to share memory
                    memset(w_addr, '\0', 1024);
                    strncpy(w_addr, buf, 1024);
                    printf("w_addr->%s\n",w_addr);
                    
                    
                    //strcat time info
                    get_cur_time(time_str);
                    strcat(buf,time_str);
                    printf("buf:%s\n",buf);
                }
                
                //子进程用于发送信息
                else if(pid == 0)
                {
                    sleep(1);
                    printf("r_addr:|%s| temp:|%s|\n",r_addr, temp);
                    
                    //swap shmat buffer
                    if(strcmp(temp,r_addr) != 0)
                    {
                        printf("swap buffer!\n");
                        get_cur_time(time_str);
                        //strcat(r_addr,time_str);
                        strcpy(temp,r_addr);
                        memset(r_addr, '\0', 255);
                        printf("temp:%s\n",temp);

                        const char *delims = { "@" };
                        char *cmd = strtok( temp, delims);
                        if( NULL == cmd )
                        {
                            printf("temp:%s parse error,no cmd!\n", temp);
                            memset(temp, '\0', 1024);
                            continue;
                        }
                        
                        
                        char* rbody = strtok( NULL, delims);
                        if( NULL == rbody )
                        {
                            printf("temp:%s parse error,no rbody!\n", temp);
                            memset(temp, '\0', 1024);
                            continue;
                        }
                        printf("cmd:%s rbody:%s\n", cmd, rbody);
                        
                        
                        if( 0 == strcmp(cmd, "reg") )
                        {
                            g_onlineLogic.regOnlineShmat(onlineShmid, rbody, clientfd);
                        }
                        
                        //send temp buffer
                        //if(send(clientfd,temp,strlen(temp),0) == -1)
                        //{
                        //    perror("send error");
                        //}
                        memset(temp, '\0', 1024);
                    }
                }
                else
                {
                    perror("fork error");
                }
            }
        }
    }
    printf("------------------------------\n");
    free(buf);
    close(sockfd);
    close(clientfd);
    return 0;
}

