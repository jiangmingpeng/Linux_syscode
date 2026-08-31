#include "CpMode.hpp"

int threadNumber = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int GetThreadNum()
{
    pthread_mutex_lock(&mutex);
    int num = threadNumber;
    threadNumber++;
    pthread_mutex_unlock(&mutex);    
    
    return num;
}


BlockQueue<int> bq(6);


void* Consumer(void* args)
{
    int* num = static_cast<int*>(args);
    while(true)
    {
        int out = 0;
        bq.Pop(&out);
        // std::cout<<"Thread-Num-"<<*num<<"消费 :"<<out<<std::endl;
        std::cout<<"消费 :"<<out<<std::endl;
        sleep(1);//很明显在消费期间生产的强度也是直接干到一样的速度
    }



    return nullptr;
}

void* Productor(void* args)
{
    int in = 1;
    int* num = static_cast<int*>(args);
    while(true)
    {
        
        bq.Push(in);
        // std::cout<<"Thread-Num-"<<*num<<"消费 :"<< in++ <<std::endl;//这个段错误的检查 等到全封装来 
        std::cout<<"生产 :"<< in++ <<std::endl;

    }
    



    return nullptr;
}




int main()
{
    
    pthread_t consumer;
    pthread_t productor;

    //至少目前按我目的跑起来了 一开始直接生产一批(因为达到最大队列限度) 然后后面生产速度与消费持平
    pthread_create(&consumer,nullptr,Consumer,(void*)GetThreadNum());
    pthread_create(&productor,nullptr,Productor,(void*)GetThreadNum());

    sleep(10);

    pthread_cancel(consumer);
    pthread_cancel(productor);

    pthread_join(consumer,nullptr);
    pthread_join(productor,nullptr);


    return 0;
}



































// void *Routine(void *args)
// {
//     char *str = static_cast<char *>(args);
//     while (true)
//     {
//         sleep(1);
//         std::cout << "tid: " << pthread_self() << "  " << str << std::endl;
//     }

//     return nullptr;
// }

// int main()
// {
//     BlockQueue<int> bq(5);

//     // pthread_t consumer;
//     // pthread_t productor;
//     // pthread_create(&consumer,nullptr,Routine,(void*)"这是消费者线程... ");
//     // pthread_create(&productor,nullptr,Routine,(void*)"这是生产者线程... ");

//     for (int i = 0; i < 10; ++i)
//     {
//         bq.Push(i); // 生产一个
//         std::cout << "sc=" <<i<<std::endl;
//         sleep(1);
//         int val;
//         bq.Pop(&val); // 消费一个
//         std::cout<< "got=" << val << std::endl;
//     }//主线程自产自销

//     // sleep(5);
//     // pthread_cancel(consumer);
//     // pthread_cancel(productor);

//     // pthread_join(consumer,nullptr);
//     // pthread_join(productor,nullptr);

//     return 0;
// }