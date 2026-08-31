#pragma once
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
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_Productor, nullptr);
        pthread_cond_init(&_Consumer, nullptr);
    }
    void Push(const T &in)
    {
        pthread_mutex_lock(&_mutex);
        while (_data.size() == _capacity)
        {
            _pro_block_num++;
            pthread_cond_wait(&_Productor, &_mutex);
            _pro_block_num--;
        }

        _data.push(in);
        if(_con_block_num > 0 )
            pthread_cond_signal(&_Consumer);//应该是生产好了之后才通知消费者
        pthread_mutex_unlock(&_mutex);

    } // 生产者方就是入队列
    void Pop(T *out)
    {

        pthread_mutex_lock(&_mutex);
        while (_data.empty())
        {
            _con_block_num++;
            pthread_cond_wait(&_Consumer, &_mutex);
            _con_block_num--;
        }

        *out = _data.front(); // 明白这是一个输出型参数
        _data.pop();
        if(_pro_block_num > 0)//刚刚消费了 看看有有没有生产者在睡眠 把它喊醒来生产了
            pthread_cond_signal(&_Productor);
        pthread_mutex_unlock(&_mutex);

    } // 消费者方就是出队列
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_Productor);
        pthread_cond_destroy(&_Consumer);
    }

private:
    std::queue<T> _data;
    int _capacity; // 这是总容量大小不是当前元素个数 别搞错了
    pthread_mutex_t _mutex;
    pthread_cond_t _Productor;
    pthread_cond_t _Consumer;

    //什么时候唤醒消费 什么时候唤醒生产
    int _pro_block_num;
    int _con_block_num;
};