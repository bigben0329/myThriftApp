//
//  OnlineLogic.cpp
//  myThriftApp
//
//  Created by momo@ben on 13-9-8.
//
//

#include "OnlineLogic.h"

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

using namespace std;


OnlineInfo::OnlineInfo()
{
    _name.clear();
    
}

void OnlineInfo::encode(std::string& content)
{
    content.clear();
    char Tmp[128] = {0};
    snprintf(Tmp, sizeof(Tmp), "%s%s%d%s",
             _name.c_str(), SPR_CHAR_ATTR
             ,_fd,SPR_CHAR_ATTR);
    content = std::string(Tmp);
}

int OnlineInfo::decode(const std::string content)
{
    string::size_type pos1 = 0, pos2;
    std::string tmp;
    
    //get name value
    pos2 = content.find(SPR_CHAR_ATTR);
    if(string::npos == pos2)
    {
        printf("no _name!\n");
        return -1;
    }
    
    tmp = content.substr(pos1, pos2 - pos1);
    printf("_name: %s\n",tmp.c_str());
    _name = tmp;
    pos1 = pos2 + 1;
    
    
    //get fd value
    pos2 = content.find(SPR_CHAR_ATTR, pos1);
    if(string::npos == pos2)
    {
        printf("no _id!\n");
        return -2;
    }
    
    tmp = content.substr(pos1, pos2 - pos1);
    printf("_fd: %s\n",tmp.c_str());
    _fd = atoi(tmp.c_str());
    pos1 = pos2 + 1;
    
    return 0;
}


COnlineLogic::COnlineLogic()
{
    mapOnline.clear();
}


COnlineLogic::~COnlineLogic()
{
    
}


key_t COnlineLogic::initOnlineShmat()
{
    key_t shmid;
    if((shmid = shmget(IPC_PRIVATE,1024,S_IRUSR|S_IWUSR)) == -1)
    {
        fprintf(stderr,"Create Share Memory Error:%s\n\a",strerror(errno));
        exit(1);
    }
    return shmid;
}

int COnlineLogic::regOnlineShmat(key_t shmid, const char* name, int fd)
{
    char *r_addr = (char*)shmat(shmid, 0, 0);
    printf("online shmat begin:%s\n",r_addr);
    
    rebuildOnlineShmat(shmid);
    typeof( mapOnline.begin() ) itInfo = mapOnline.find(name);
    if( itInfo == mapOnline.end() )
    {
        OnlineInfo info;
        info._name = name;
        info._fd = fd;
        mapOnline.insert(make_pair(info._name, info));
        
        std::string content;
        calOnlineShmatBuffer(content);
        memcpy(r_addr, (char*)content.c_str(), content.length());
    }
    
    printf("online shmat end:%s\n",r_addr);
    return 0;
}


int COnlineLogic::rebuildOnlineShmat(key_t shmid)
{
    char *r_addr = (char*)shmat(shmid, 0, 0);
    printf("%s\n",r_addr);
    mapOnline.clear();
    
    std::string content = std::string(r_addr);
    string::size_type pos1 = 0, pos2;
    pos2 = content.find(SPR_CHAR_ITEM);
    while (string::npos != pos2)
    {
        std::string temp = content.substr(pos1, pos2 - pos1);
        printf("temp: %s\n",temp.c_str());
        
        OnlineInfo info;
        if( 0 == info.decode(temp))
        {
            mapOnline.insert(make_pair(info._name, info));
        }
        
        pos1 = pos2 + 1;
        pos2 = content.find(SPR_CHAR_ITEM, pos1);
    }
    
    return 0;
}


int COnlineLogic::calOnlineShmatBuffer(std::string& content)
{
    content.clear();
    
    std::string item;
    typeof( mapOnline.begin() ) itInfo = mapOnline.begin();
    for(; itInfo != mapOnline.end(); itInfo++)
    {
        itInfo->second.encode(item);
        content+=item;
        content+=SPR_CHAR_ITEM;
    }
}

