#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <cstdlib>
void Func(int sig_node)
{
    std::cout << "Doing user's setting func.... " << "sig_node: " << sig_node << "Doing task: " << getpid() << std::endl;
    // exit(1);
    // alarm(1);
    unsigned int n = alarm(2);
    std::cout<<"上一个闹钟还剩下的时间: "<<n<<std::endl;
}


int main()
{
    signal(SIGALRM,Func);
    alarm(100);
    int cnt = 1;
    while(true)
    {
        std::cout<<"Cnt: "<<cnt<<" pid: "<<getpid()<<std::endl;
        cnt++;
        sleep(1);
    }


    // signal(SIGABRT, Func);
    // while (true)
    // {
    //     sleep(1);
    //     abort();
    // }

    // signal(2, Func); // 先对2号信号进⾏捕捉
    // // 每隔1s，给自己发送2号信号
    // while (true)
    // {
    //     sleep(1);
    //     raise(2);
    // }

    // signal(2,Func);
    // // for(int signalnum = 1;signalnum<=34;signalnum++)
    // // {
    // //     signal(signalnum,SIG_IGN);
    // // }

    // while(true)
    // {
    //     std::cout<<"test signal..."<<" pid:"<<getpid()<<std::endl;
    //     sleep(1);

    //     // int n = 10;
    //     // n/=0;
    //     // int* p = nullptr;
    //     // *p = 5;
    // }
    return 0;
}