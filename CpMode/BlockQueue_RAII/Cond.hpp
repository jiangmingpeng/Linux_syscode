#pragma once
#include "Mutex.hpp"
#include <iostream>

class Cond
{
public:
    Cond()
    {
        pthread_cond_init(&_cond,nullptr);
    }

    void Wait(MutexMould& mutex)
    {
        pthread_cond_wait(&_cond,mutex.Ptr());
    }

    void Signal()
    {
        pthread_cond_signal(&_cond);//应该是谁调用谁就是谁的 先保留想法
    }

    void Broadcast()
    {
        pthread_cond_broadcast(&_cond);
    }

    ~Cond()
    {
        pthread_cond_destroy(&_cond);
    }
private:
    pthread_cond_t _cond;
};