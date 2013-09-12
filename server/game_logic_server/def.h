//
//  server.h
//  myThriftApp
//
//  Created by momo@ben on 13-9-11.
//
//

#ifndef myThriftApp_def_h
#define myThriftApp_def_h

using namespace std;

#define PERM S_IRUSR|S_IWUSR
#define MYPORT 3490  //宏定义定义通信端口
#define BACKLOG 10 //宏定义，定义服务程序可以连接的最大客户数量

//宏定义，当客户端连接服务端时，想客户发送此欢迎字符串
#define WELCOME "|----------Welcome to the plane room! ----------|"


#define bool int
#define true 1
#define false 0

#define LISTENQ     1024         /* 2nd argument to listen () */
#define MAXLINE     4096         /* max text line length */

#ifndef INFTIM
#define INFTIM          (-1)     /* infinite poll timeout */
#ifdef HAVE_POLL_H
#define INFTIM_UNPH              /* tell unpxti.h we defined it */
#endif
#endif



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

#endif
