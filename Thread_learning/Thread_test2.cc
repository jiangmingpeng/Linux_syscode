#include <iostream>
#include <pthread.h>
#include <unistd.h>

void* Routine(void* args)
{
    std::cout<<static_cast<const char*>(args)<<std::endl;
    //把自己分离出来
    // pthread_detach(pthread_self());
    // return nullptr;
    void* ret = (void*)"这个新线程退出来了...\n"; 
    pthread_exit(ret);
}

int main()
{

    pthread_t tid;
    char* name = "我是thread-1";
    pthread_create(&tid,nullptr,Routine,static_cast<void*>(name));
    //在主线程内把某一个新线程给分离掉 后续自动释放资源不用再由主线程来等待
    pthread_detach(tid);
    sleep(1);//很重要，要让线程先分离，再等待 否则由于调度的原因 还没到新线程就又在主线程里回收了 导致分离不成功
    void* retval = nullptr;
    pthread_join(tid,&retval);
    if(retval == nullptr)
    {
        std::cout<<"获取线程结束返回值失败..."<<std::endl;
    }
    else
    {
        std::cout<<(char*)retval<<std::endl;//拿到了线程结束时候的返回值
    }

    while(true)
    {
        std::cout<<"这是主线程....."<<std::endl;
        sleep(1);
    }

    return 0;
}