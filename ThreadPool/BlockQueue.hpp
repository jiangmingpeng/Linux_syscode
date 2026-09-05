#pragma once
#include "Cond.hpp"
#include <iostream>
#include <queue>
#include <pthread.h>
#include <unistd.h>

#define CAP 16
// 基于阻塞队列为容器的生产者消费者模型
template <class T>
class BlockQueue
{
public:
    BlockQueue(int cap = CAP) : _capacity(cap)
    {
        _pro_block_num = 0;
        _con_block_num = 0;
    }
    void Push(const T &in)
    {
        {
            MutexGuard lock(_mutex);
            while (_data.size() == _capacity)
            {
                _pro_block_num++;
                _Productor.Wait(_mutex);
                _pro_block_num--;
            }

            _data.push(in);
            if (_con_block_num > 0)
                _Consumer.Signal();
        }

    } // 生产者方就是入队列
    void Pop(T *out)
    {

        {
            MutexGuard lock(_mutex);
            while (_data.empty())
            {
                _con_block_num++;
                _Consumer.Wait(_mutex);
                _con_block_num--;
            }

            *out = _data.front(); // 明白这是一个输出型参数
            _data.pop();
            if (_pro_block_num > 0) // 刚刚消费了 看看有有没有生产者在睡眠 把它喊醒来生产了
                _Productor.Signal();
        }

    } // 消费者方就是出队列
    ~BlockQueue()
    {
    }

private:
    std::queue<T> _data;
    int _capacity;
    MutexMould _mutex;
    Cond _Productor;
    Cond _Consumer;

    int _pro_block_num;
    int _con_block_num;
};