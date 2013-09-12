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
#include<string>
#include<string.h>

using namespace std;


OnlineInfo::OnlineInfo()
{
    _name.clear();
    
}

void OnlineInfo::encode(std::string& content)
{
    content.clear();
    char Tmp[128] = {0};
    snprintf(Tmp, sizeof(Tmp), "%s%s%d%s%d%s%d%s",
             _name.c_str(), SPR_CHAR_ATTR
             ,_fd,SPR_CHAR_ATTR
             ,_stat,SPR_CHAR_ATTR
             ,_vsfd,SPR_CHAR_ATTR);
    
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
    //printf("_name: %s\n",tmp.c_str());
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
    //printf("_fd: %s\n",tmp.c_str());
    _fd = atoi(tmp.c_str());
    pos1 = pos2 + 1;
    
    
    //get stat value
    pos2 = content.find(SPR_CHAR_ATTR, pos1);
    if(string::npos == pos2)
    {
        printf("no _stat!\n");
        return -3;
    }
    
    tmp = content.substr(pos1, pos2 - pos1);
    //printf("_stat: %s\n",tmp.c_str());
    _stat = atoi(tmp.c_str());
    pos1 = pos2 + 1;

    
    //get vsfd value
    pos2 = content.find(SPR_CHAR_ATTR, pos1);
    if(string::npos == pos2)
    {
        printf("no _vsfd!\n");
        return -3;
    }
    
    tmp = content.substr(pos1, pos2 - pos1);
    //printf("_vsfd: %s\n",tmp.c_str());
    _vsfd = atoi(tmp.c_str());
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


int COnlineLogic::initOnlineShmat(key_t shmid)
{
    _shAddr = (char*)shmat(shmid, 0, 0);
    _shmid = shmid;
    return 0;
}

int COnlineLogic::updateOnlineStat(int fd, ONLINE_STAT stat)
{
    printf("updateOnlineStat shmat begin:%s fd:%d stat:%d\n",
           _shAddr, fd, stat);
    
    bool bUpdate = false;
    rebuildOnlineShmat();
       
    typeof( mapOnline.begin() ) itInfo = mapOnline.find(fd);
    if( itInfo != mapOnline.end())
    {
        itInfo->second._stat = stat;
        itInfo->second._fd = fd;
        bUpdate = true;
    }
    
    if(bUpdate)
    {
        std::string content;
        calOnlineShmatBuffer(content);
        memcpy(_shAddr, (char*)content.c_str(), content.length());
        rebuildOnlineShmat();
    }
    
    printf("updateOnlineStat shmat end:%s\n",_shAddr);
    return 0;

}


int COnlineLogic::getOnlineStat( int fd)
{
    //printf("getOnlineStat shmat begin:%s\n",_shAddr);
    
    rebuildOnlineShmat();
    typeof( mapOnline.begin() ) itInfo = mapOnline.find(fd);
    if( itInfo != mapOnline.end())
    {
        return itInfo->second._stat;
    }
    else
    {
        return ONLINE_STAT_NEW;
    }
    
    //printf("getOnlineStat shmat end:%s\n",_shAddr);
}



int COnlineLogic::regOnlineShmat(const char* name, int fd)
{
    printf("regOnlineShmat shmat begin:%s name:%s fd:%d\n",
           _shAddr, name, fd);
    
    bool bUpdate = false;
    rebuildOnlineShmat();
    typeof( mapOnline.begin() ) itInfo = mapOnline.find(fd);
    if( itInfo == mapOnline.end())
    {
        OnlineInfo info;
        info._name = name;
        info._fd = fd;
        info._stat = ONLINE_STAT_ON;
        mapOnline.insert(make_pair(info._fd, info));
        
        bUpdate = true;
    }
    //update the same user
    else if (itInfo->second._name != name)
    {
        itInfo->second._name = name;
        itInfo->second._stat = ONLINE_STAT_ON;
        
        bUpdate = true;
    }
    
    
    //update shmat info
    if( bUpdate )
    {
        std::string content;
        calOnlineShmatBuffer(content);
        memcpy(_shAddr, (char*)content.c_str(), content.length());
        rebuildOnlineShmat();
    }
    
    printf("regOnlineShmat shmat end:%s\n",_shAddr);
    return 0;
}


int COnlineLogic::rebuildOnlineShmat()
{
    //printf("rebuildOnlineShmat shmat begin:%s\n",_shAddr);
    mapOnline.clear();
    
    std::string content = std::string(_shAddr);
    string::size_type pos1 = 0, pos2;
    pos2 = content.find(SPR_CHAR_ITEM);
    while (string::npos != pos2)
    {
        std::string temp = content.substr(pos1, pos2 - pos1);
        
        OnlineInfo info;
        if( 0 == info.decode(temp))
        {
            mapOnline.insert(make_pair(info._fd, info));
        }
        
        pos1 = pos2 + 1;
        pos2 = content.find(SPR_CHAR_ITEM, pos1);
    }
    
    //printf("rebuildOnlineShmat shmat end:%s\n",_shAddr);
    return 0;
}


int COnlineLogic::displayOnlineShmat()
{
    printf("displayOnlineShmat shmat begin:%s\n",_shAddr);
    
    std::string content = std::string(_shAddr);
    string::size_type pos1 = 0, pos2;
    pos2 = content.find(SPR_CHAR_ITEM);
    while (string::npos != pos2)
    {
        std::string temp = content.substr(pos1, pos2 - pos1);
        printf("display: %s\n",temp.c_str());
        pos1 = pos2 + 1;
        pos2 = content.find(SPR_CHAR_ITEM, pos1);
    }
    
    return 0;
}


int COnlineLogic::getOnlineList(std::string& list)
{
    printf("getOnlineList shmat begin:%s\n",_shAddr);
    
    list.clear();
    std::string content = std::string(_shAddr);
    string::size_type pos1 = 0, pos2;
    pos2 = content.find(SPR_CHAR_ITEM);
    while (string::npos != pos2)
    {
        std::string temp = content.substr(pos1, pos2 - pos1);
        printf("display: %s\n",temp.c_str());
        
        OnlineInfo info;
        if( 0 == info.decode(temp) && info._stat == ONLINE_STAT_ON )
        {
            list+=info._name;
            list+="\n";
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
    return 0;
}

int COnlineLogic::vs(int fd,const std::string vsname)
{
    printf("COnlineLogic vs fd:%d name:%s\n", fd, vsname.c_str());
    
    bool bUpdate = false, bFound = false;
    int iEnemyFd = 0;
    rebuildOnlineShmat();
    typeof( mapOnline.begin() ) itMap = mapOnline.begin();
    for( ; itMap != mapOnline.end(); itMap++ )
    {
        if( itMap->second._name == vsname )
        {
            itMap->second._vsfd = fd;
            iEnemyFd = itMap->second._fd;
            bFound = true;
        }
    }
    
    
    if( bFound )
    {
        typeof( mapOnline.begin() ) itMy = mapOnline.find(fd);
        if( itMy != mapOnline.end() )
        {
            itMy->second._vsfd = iEnemyFd;
            bUpdate = true;
        }
    }
    
    
    if( bUpdate )
    {
        std::string content;
        calOnlineShmatBuffer(content);
        memcpy(_shAddr, (char*)content.c_str(), content.length());
        rebuildOnlineShmat();
    }
    
    printf("COnlineLogic vs end bUpdate:%d\n", bUpdate);
    return 0;
}


int COnlineLogic::getVsFD(int fd)
{
    printf("getVsFD begin fd:%d\n", fd);
    
    rebuildOnlineShmat();
    typeof( mapOnline.begin() ) itMy = mapOnline.find(fd);
    if( itMy != mapOnline.end() )
    {
        return itMy->second._vsfd;
    }
    else
    {
        printf("can not found for myfd %d\n", fd);
        return -1;
    }

    printf("getVsFD end\n");
}

