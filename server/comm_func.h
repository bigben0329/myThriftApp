//
//  comm_func.h
//  myThriftApp
//
//  Created by momo@ben on 13-9-11.
//
//

#ifndef __myThriftApp__comm_func__
#define __myThriftApp__comm_func__

class CCommFunc
{
public:
    CCommFunc();
    ~CCommFunc();
    
    static void itoa(int i,char*string);
    static void get_cur_time(char * time_str);
};


#endif /* defined(__myThriftApp__comm_func__) */
