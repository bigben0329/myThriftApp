//
//  server.h
//  myThriftApp
//
//  Created by momo@ben on 13-9-11.
//
//

#ifndef myThriftApp_server_h
#define myThriftApp_server_h

using namespace std;

#define PERM S_IRUSR|S_IWUSR
#define MYPORT 3490  //宏定义定义通信端口
#define BACKLOG 10 //宏定义，定义服务程序可以连接的最大客户数量

//宏定义，当客户端连接服务端时，想客户发送此欢迎字符串
#define WELCOME "|----------Welcome to the plane room! ----------|"

#include "json.h"

#define bool int
#define true 1
#define false 0

#define  GET_JSON_STRING(p , len) \
    Json::Value req;\
    if (len > 0) \
    {\
    Json::Reader lc_ReqReader;\
    bool lb_PaseRet = false;\
    char cTempEnd = *(p + len);\
    *(p + len) = '\0';\
    lb_PaseRet = lc_ReqReader.parse(p, p + len , req);\
    if (!lb_PaseRet || !req.isObject())\
    {\
    *(p + len) = cTempEnd;\
    return -1;\
    }\
    *(p + len) = cTempEnd;\
    }


class CService
{
public:
    CService();
    ~CService();

public:
    //创建共享内存
    key_t shm_create();

    //端口绑定函数,创建套接字，并绑定到指定端口
    int bindPort(unsigned short int port);
    
    //发送回包信息
    int sendResponse(int fd, std::string cmd, std::string rps);
    
};

#endif