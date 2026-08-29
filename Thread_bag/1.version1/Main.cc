#include "Thread.hpp"
#include <unistd.h>
#include <vector>


void *Routine()
{
    while (true)
    {
        std::cout << "正在执行业务任务..." << std::endl;
        std::cout << "you are right..." << std::endl;
        std::cout << "yes i'm fine..." << std::endl;

        sleep(1);
    }
    return nullptr;
}

int main()
{
    // ThreadMould::Thread t(Routine);
    // t.Start();
    // usleep(1000);

    // // t.Datach();

    // // t.Die();

    // // t.Join();
    // while(true)
    // {
    //     std::cout<<"主线程在执行...."<<std::endl;
    //     sleep(1);
    // }

    // ThreadMould::Thread t(Routine);
    // sleep(1);
    // t.Start();
    // // t.Detach();
    // sleep(5);
    // t.Die();
    // sleep(1);
    // t.Join();
    // t.PrintInfo();

    std::vector<ThreadMould::Thread> threads; 
    int threadNum = 0;
    std::cin>>threadNum;
    for(int i = 0;i<threadNum;i++)
    {
        threads.emplace_back(Routine);
    }
    for (auto &t : threads)
    {
        t.Start();
        // sleep(1);
    }

    sleep(5);
    for (auto &t : threads)
    {
        t.Die();
    }

    for (auto &t : threads)
    {
        t.Join();
        t.PrintInfo();
        // sleep(1);
    }
    return 0;
}