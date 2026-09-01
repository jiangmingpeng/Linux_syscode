#pragma once
#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <vector>

class Sem
{
public:
    Sem(int SemVal)
    {
        if (SemVal >= 0)
        {
            int n = sem_init(&_sem, 0, SemVal); // 信号量对象 线程or进程共享开关 信号量大小
            (void)n;
        }
    }

    void P() // 申请信号量操作
    {
        int n = sem_wait(&_sem);
        (void)n;
    }

    void V() // 释放信号量
    {
        int n = sem_post(&_sem);
        (void)n;
    }
    ~Sem()
    {
        sem_destroy(&_sem);
    }

private:
    sem_t _sem;
};