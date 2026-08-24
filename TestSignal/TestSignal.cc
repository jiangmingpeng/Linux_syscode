#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
int flag = 0;
void handler0(int signode)
{
    std::cout<<"这是4号信号反应的测试函数.....pid: "<<getpid()<<std::endl;
    sleep(2);
    exit(2);
}

void handler(int signode)
{
    std::cout<<"这是2号信号反应的测试函数.....pid: "<<getpid()<<std::endl;
    flag = 1;
    sleep(12);
    // exit(1);
}

int main()
{
    // signal(SIGINT,handler);
    sigset_t set;
    sigaddset(&set,4);
    sigaddset(&set,5);
    sigaddset(&set,6);
    struct sigaction act;
    struct sigaction old_act;
    act.sa_handler = handler;
    act.sa_mask = set;
    sigaction(SIGINT,&act,&old_act);
    signal(4,handler0);
    std::cout<<"pid: "<<getpid()<<std::endl;
    while(!flag);
    std::cout<<"flag循环结束...."<<std::endl;


    return 0;
}