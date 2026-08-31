#pragma once
#include <iostream>
#include <pthread.h>



class MutexMould
{
public:
    MutexMould()
    {
        pthread_mutex_init(&_mutex,nullptr);
    }

    pthread_mutex_t* Ptr()
    {
        return &_mutex;
    }

    void Lock()
    {
        (pthread_mutex_lock(&_mutex));
    } 
    void UnLock()
    {
        (pthread_mutex_unlock(&_mutex));
    }
    ~MutexMould()
    {
        pthread_mutex_destroy(&_mutex);
    }
private:

    pthread_mutex_t _mutex;//先进行全局的这种封装看看

};


//RAII内存管理思想写法

class MutexGuard
{
public:
    MutexGuard(MutexMould& lock):_lockguard(lock)
    {
        _lockguard.Lock();
    }
    ~MutexGuard()
    {
        _lockguard.UnLock();
    }
private:
    MutexMould& _lockguard;//因为是类用引用开销会小很多
};