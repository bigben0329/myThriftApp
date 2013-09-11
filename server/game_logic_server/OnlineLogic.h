//
//  OnlineLogic.h
//  myThriftApp
//
//  Created by momo@ben on 13-9-8.
//
//

#ifndef __myThriftApp__OnlineLogic__
#define __myThriftApp__OnlineLogic__

#include <string>
#include <map>
#include<sys/shm.h>

#define SPR_CHAR_ITEM "|"
#define SPR_CHAR_ATTR ","

enum ONLINE_STAT
{
    ONLINE_STAT_NEW = 0,
    ONLINE_STAT_ON = 1,
    ONLINE_STAT_OFF = 2
};

//reg user info
struct OnlineInfo
{
public:
    
    OnlineInfo();
    
    std::string _name;
    int _fd;
    int _stat;
    
public:
    void encode(std::string& content);
    int decode(const std::string content);
    
};


class COnlineLogic
{
public:
    COnlineLogic();
    ~COnlineLogic();
    
    int initOnlineShmat(key_t shmid);
    int displayOnlineShmat();
    int getOnlineList(std::string& content);
    
    int getOnlineStat(int fd);
    int updateOnlineStat(int fd, ONLINE_STAT stat);
    
    int regOnlineShmat(const char* name, int fd);
    int rebuildOnlineShmat();
    int calOnlineShmatBuffer(std::string& content);

private:
    
    char* _shAddr;
    key_t _shmid;
    std::map<int,OnlineInfo> mapOnline;
};

#endif /* defined(__myThriftApp__OnlineLogic__) */
