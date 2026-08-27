#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

#include <vector>
#include "Thread_task.hpp"

// 新线程的创建主体
void *New_thread(void *arg)
{
    // int cnt = 10;
    // while (cnt--)
    while(true)
    {
        std::cout << "---------------------------" << std::endl;
        // std::cout << "This is a new thread..." << " thread_id:" << pthread_self()
        //           << static_cast<char *>(arg) << std::endl;
        std::cout << "This is a new thread..." << " thread_id:" << pthread_self() << std::endl;
        std::cout << static_cast<Task *>(arg)->Wayadd() << std::endl;
        sleep(1);
    }
    // delete static_cast<char *>(arg);
    // void* ret;
    // pthread_exit(ret);//这个退出函数和等待函数再学习一遍
    return nullptr;
}

int main()
{
    // 创建多线程
    int To_ct_threadNum = 0;
    srand((time(nullptr) ^ getpid())); // 根据时间戳种下随机数种子
    std::vector<pthread_t> tids;
    std::cin >> To_ct_threadNum;
    for (int i = 0; i < To_ct_threadNum; i++)
    {
        pthread_t thread_t;      // 其实就是线程id--具体用来控制新线程的
        int x = rand() % 10 + 1; // 生成随机数
        usleep(2545);
        int y = rand() % 7 + 2;
        Task *t = new Task(x, y); // 注意线程传的void* 即任意指针类型
        // char threadName[64];
        char *threadName = new char[64];
        sprintf(threadName, "这是创建的第%d个线程\n", i + 1);
        // pthread_create(&thread_t,nullptr,New_thread,threadName);//放回的是创建的新线程的线程id
        pthread_create(&thread_t, nullptr, New_thread, t); // 放回的是创建的新线程的线程id
        tids.push_back(thread_t);
        // sleep(1);
    }
    tids.push_back(pthread_self());//把主线程的id也拿进来
    for (auto &tid : tids) // 把创建保存好的线程id打印出来
    {
        printf("tid:  0x%lx\n", tid);
    }
    // 主线程继续跑
    // while (true)
    int cnt = 10;
    while (cnt--)
    {
        std::cout << "This is main thread..." << "  thread_id:" << pthread_self() << std::endl;
        sleep(1);
        int who = rand() % tids.size(); // 随机杀死一个线程 直到全部杀死
        if (tids[who] == -1)
            continue;
        else
        {
            //终止线程
            pthread_cancel(tids[who]);//传的是线程id
            tids[who] = -1;//删除之后置为空
        }
    }
    // int who = rand() % tids.size(); // 随机杀死一个线程 直到全部杀死
    // void *ret;
    // pthread_join(tids[who], &ret);

    return 0;
}
