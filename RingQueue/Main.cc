#include "RingQueue.hpp"
#include "Tasks.hpp"
#include "Thread.hpp"
#include <ctime>
#include <string>
#include <iostream>


using namespace ThreadMould;

int main()
{
    srand(time(nullptr)^getpid());
    // RingQueue<int>* rq = new RingQueue<int>;
    RingQueue<tasks_t>* rq = new RingQueue<tasks_t>;
    
    pthread_t c,p;


    Thread Consumer([&rq]()
    {
        while (true)
        {
            {
                sleep(1);
                // int out = 0;
                tasks_t out = nullptr;
                rq->Pop(&out);//输出型参数
                out();
                // std::cout<<"消费: "<<out<<std::endl;
            }

        }
    });
    Thread Productor([&rq]()
    {
        // int in = 0;
        std::vector<tasks_t> tasks = TaskList();
        int size = tasks.size();
        while(true)
        {
            {
                int index = rand()%size;
                rq->Push(tasks[index]);//输入型参数
                std::cout<<"生产的任务: "<<index<<std::endl;
            } 

        } 
    });

    Consumer.Start();
    Productor.Start();

    Consumer.Join();
    Productor.Join();


    // pthread_create(&c,nullptr,Consumer,rq);
    // pthread_create(&p,nullptr,Productor,rq);

    // pthread_join(c,nullptr);
    // pthread_join(p,nullptr);


    return 0;
}