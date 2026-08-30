#include <iostream>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <mutex>
#include "Mutex.hpp"

// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
MutexMould _mutex;

int ticket = 1000;

void *BuyTicket(void *args)
{
    char *id = static_cast<char *>(args);
    while (true)
    {
        {
            MutexGuard mutex(_mutex);//通过在作用域内结束后调用析构函数来实现上锁和解锁
            // pthread_mutex_lock(&mutex);
            // mutex.Lock();
            if (ticket > 0)
            {
                usleep(1000);
                std::cout << id << "sells tickets: " << ticket << std::endl;
                ticket--;
                // mutex.UnLock();
                // pthread_mutex_unlock(&mutex);
            }
            else
            {
                // mutex.UnLock();
                // pthread_mutex_unlock(&mutex);
                break;
            }
        }
    }
    return nullptr;
}

int main()
{
    int threadNum = 0;
    std::cin >> threadNum;
    std::vector<pthread_t> tids;
    for (int i = 0; i < threadNum; i++)
    {
        pthread_t tid;
        char threadName[64];
        sprintf(threadName, "Thread-Name-%d ", i + 1);
        pthread_create(&tid, nullptr, BuyTicket, (void *)threadName);
        tids.emplace_back(tid);
        // usleep(1000);
    }
    // sleep(5);
    if (ticket == 0)
    {
        for (auto &tid : tids)
        {
            pthread_cancel(tid); // 回收线程
        }
    }

    for (auto &tid : tids)
    {
        pthread_join(tid, nullptr); // 不关心退出信息 ---等待线程
    }

    while (true)
    {
        std::cout << "主线程不参与抢票...." << std::endl;
        sleep(1);
    }
    return 0;
}