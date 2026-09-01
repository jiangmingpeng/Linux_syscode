#pragma once
#include "Sem.hpp"
#include "Mutex.hpp"

#define CAP 5

template <typename T>
class RingQueue
{
public:
    RingQueue(int capacity = CAP)
        : _ring(capacity), _cap(capacity), _consumer_step(0), _productor_step(0), _blank_sem(capacity), _res_sem(0)
    {
    }

    void Push(T &in)
    {
        _blank_sem.P(); // 锁域不包括PV操作是因为申请资源的时候是可以多线程一起 能不能申请到那另讲 但是申请到的就必须以互斥的方式来进行访问临界资源
        {               // 注意理解这个环形队列的生产和消费之间的关系 尤其是我消费资源 其实是给另一方提供了空格子
            MutexGuard lockguard(_pmutex);
            _ring[_productor_step++] = in;
            _productor_step %= _cap;
        }
        _res_sem.V();
    }

    void Pop(T *out)
    {
        _res_sem.P();
        {
            MutexGuard lockguard(_cmutex);
            *out = _ring[_consumer_step++];
            _consumer_step %= _cap;
        }
        _blank_sem.V();
    }

    ~RingQueue()
    {
    }

private:
    std::vector<T> _ring;
    int _cap;

    int _consumer_step;
    int _productor_step;

    Sem _blank_sem; // 空格子信号量
    Sem _res_sem;   // 资源信号量

    MutexMould _pmutex;
    MutexMould _cmutex;
};