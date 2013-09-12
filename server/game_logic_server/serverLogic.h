//
//  serverLogic.h
//  myThriftApp
//
//  Created by momo@ben on 13-9-12.
//
//

#ifndef __myThriftApp__serverLogic__
#define __myThriftApp__serverLogic__

#include <sys/types.h>  //数据类型定义


class CService
{
public:
    CService();
    ~CService();
    
public:
    //创建共享内存
    key_t shm_create();
    
    //端口绑定函数,创建套接字，并绑定到指定端口
    int BindPort(unsigned short int port);
    int Listen(int fd);
    int Accept(int fd);
    
    //发送回包信息
    int sendResponse(int fd, std::string cmd, std::string rps);
    
};



#endif /* defined(__myThriftApp__serverLogic__) */
