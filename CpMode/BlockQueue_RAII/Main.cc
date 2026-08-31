#include "BlockQueue.hpp"
#include "Tasks.hpp"
#include "Thread.hpp"
#include <memory>
#include <ctime>

using namespace ThreadMould;

int main()
{
    // 用智能指针创建对象
    std::unique_ptr<BlockQueue<tasks_t>> bq = std::make_unique<BlockQueue<tasks_t>>(); // 用法参考普通指针 功能更强
    srand(time(nullptr)^getpid());
    // 中括号(捕获变量)里面的&是引用外面的变量的意思
    Thread Consumer([&bq]()
    {//变量名在初始化的时候就已经
        while (true)
        {
            tasks_t out ;
            //获取任务
            bq->Pop(&out);
            //执行任务
            out();
            sleep(1); 
            std::cout<<"消费者正在执行任务"<<std::endl;
        } 
    });
    Thread Productor([&bq]()
    {
        std::vector<tasks_t> Task_list = TaskList() ;
        std::cout << "Task_list size: " << Task_list.size() << std::endl; // 加这行调试
        while (true)
        {
            usleep(1);
            int index = rand()%Task_list.size();
            bq->Push(Task_list[index]);
            std::cout<<"生产者正在产出任务"<<std::endl;
        } 
    });

    //以上是单消费者和单生产者线程的创建 里面各自回调一个函数

    Consumer.Start();
    Productor.Start();

    sleep(15);

    Consumer.Join();
    Productor.Join();

    Consumer.PrintInfo();
    Productor.PrintInfo();

    // pthread_t consumer;
    // pthread_t productor;

    // // 至少目前按我目的跑起来了 一开始直接生产一批(因为达到最大队列限度) 然后后面生产速度与消费持平
    // pthread_create(&consumer, nullptr, Consumer, (void *)GetThreadNum());
    // pthread_create(&productor, nullptr, Productor, (void *)GetThreadNum());

    // sleep(10);

    // pthread_cancel(consumer);
    // pthread_cancel(productor);

    // pthread_join(consumer, nullptr);
    // pthread_join(productor, nullptr);

    return 0;
}