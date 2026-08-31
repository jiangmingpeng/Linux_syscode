#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <vector>  

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int cnt = 1000;

// void* Routine(void* args)  //这种情况会存在数据不一致问题 尽管判断了大于0 具体原因见笔记
// {

//     char* str = static_cast<char*>(args);
//     while(true)
//     {
//         if(cnt>0)
//         {
//             usleep(1000);
//             cnt--;
//             std::cout<<"我是"<<str<<"  我在向显示器上打印消息"<<"cnt: "<<cnt<<std::endl;

//         }
//         else
//         {
//             break;
//         }
//     }
//     return nullptr;
// }
void *Routine(void *args)
{

    char *str = static_cast<char *>(args);
    while (true)
    {
        pthread_mutex_lock(&mutex);
        if (cnt > 0)
        {
            usleep(1000);
            cnt--;
            std::cout << "我是" << str << "  我在向显示器上打印消息  " << "cnt: " << cnt << std::endl;
            pthread_cond_wait(&cond,&mutex);//1.原子地释放 mutex（解锁） 2.将当前线程挂起到 cond 的等待队列中（睡眠）
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    delete str;
    return nullptr;
}

int main()
{
    std::vector<pthread_t> tids;
    int threadNum = 0;
    std::cin >> threadNum;
    for (int i = 0; i < threadNum; i++)
    {
        pthread_t Productor;
        // char ProthreadName[64];
        char* ProthreadName = new char[64];
        sprintf(ProthreadName, "生产者线程-%d ", i + 1);
        pthread_create(&Productor, nullptr, Routine, ProthreadName);
        tids.emplace_back(Productor);
    }
    for (int i = 0; i < threadNum; i++)
    {
        pthread_t Consumer;
        // char *ConthreadName;这样子写可能出现野指针错误 导致越界访问发生段错误
        // char ConthreadName[64];//这样子写由于共享问题 会出现数据覆盖问题
        char* ConthreadName = new char[64];
        sprintf(ConthreadName, "消费者线程-%d ", i + 1);
        pthread_create(&Consumer, nullptr, Routine, ConthreadName);
        tids.emplace_back(Consumer);
    }


    for(;;)
    {
        // pthread_cond_broadcast(&cond);//唤醒所有正在等待的线程
        pthread_cond_signal(&cond);//唤醒任意一个线程(具体是谁看调度器)
        sleep(2);
    }

    // sleep(10);
    for (auto &tid : tids)
    {
        pthread_cancel(tid);
    }

    for (auto &tid : tids)
    {
        pthread_join(tid, nullptr);
    }

    while (true)
    {
        std::cout << "我是主线程,我在向显示器上打印消息" << std::endl;
        sleep(1);
    }
    return 0;
}