//
//  OnlineLogic.h
//  myThriftApp
//
//  Created by momo@ben on 13-9-8.
//
//

#ifndef __myThriftApp__OnlineLogic__
#define __myThriftApp__OnlineLogic__

#include <iostream>
#include <string>
#include <map>
#include<sys/shm.h>

#define SPR_CHAR_ITEM "|"
#define SPR_CHAR_ATTR ","

//reg user info
struct OnlineInfo
{
public:
    
    OnlineInfo();
    
    std::string _name;
    int _fd;
    
public:
    void encode(std::string& content);
    int decode(const std::string content);
    
};


class COnlineLogic
{
public:
    COnlineLogic();
    ~COnlineLogic();
    
    key_t initOnlineShmat();
    int regOnlineShmat(key_t shmid, const char* name, int fd);
    int rebuildOnlineShmat(key_t shmid);
    int calOnlineShmatBuffer(std::string& content);

private:
    std::map<std::string,OnlineInfo> mapOnline;
};

#endif /* defined(__myThriftApp__OnlineLogic__) */
