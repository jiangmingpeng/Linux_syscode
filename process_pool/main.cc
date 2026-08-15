#include "process_pool.h"

int main()
{
    // 1.创建进程池
    Process_pool pp;
    pp.Init(DoTask);
    pp.Debug();//查看进程创建情况

    // 2.父进程控制子进程
    pp.Run(100);

    //3.关闭管道并且回收子进程
    pp.Quit();


    return 0;
}