//
//  comm_func.cpp
//  myThriftApp
//
//  Created by momo@ben on 13-9-11.
//
//

#include "comm_func.h"
#include <iostream>

CCommFunc::CCommFunc()
{

}

CCommFunc::~CCommFunc()
{

}

//转换函数，将int类型转换成char *类型
void CCommFunc::itoa(int i,char*string)
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
void CCommFunc::get_cur_time(char * time_str)
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
    CCommFunc::itoa(p_curtime->tm_hour,time_tmp);
    strcat(time_str,time_tmp);
    strcat(time_str,":");
    CCommFunc::itoa(p_curtime->tm_min,time_tmp);
    strcat(time_str,time_tmp);
    strcat(time_str,":");
    CCommFunc::itoa(p_curtime->tm_sec,time_tmp);
    strcat(time_str,time_tmp);
    strcat(time_str,")");
    free(time_tmp);
}