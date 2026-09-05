#pragma once
#include <queue>
#include "Thread.hpp"
#include "Logger.hpp"
#include "Mutex.hpp"
#include "Cond.hpp"

namespace ThreadPoolMould
{

    using namespace LoggerMould;
    using namespace ThreadMould;

    const int defaultNum = 5;
    template <typename T>
    class ThreadPool
    {
    private:
        void *HandlerRoutine() // 在这里是可以拿到任务队列
        {
            char threadName[64] = "\0";
            pthread_getname_np(pthread_self(), threadName, sizeof(threadName));

            while (true) // 既然线程会跑到这里来 那么这里自然是执行任务的地方
            {
                T Task;
                {      
                    MutexGuard lock(_mutex);
                    while (_task_queue.empty() && _is_Running)
                    {
                        _th_sleep_num++;
                        _cond.Wait(_mutex);
                        _th_sleep_num--;
                    }
                    if (!_is_Running && _task_queue.empty())// 同样需要考虑到要回收时在执行的等任务执行完之后再停摆
                    {
                        // _mutex.UnLock();break 后自然会进行解锁 否则这里构成双解锁
                        break;
                    }
                    Task = _task_queue.front();
                    _task_queue.pop();
                }
                LOG(LogLevel::DEBUG) << "Tasks were done...";
                Task();
            }
            LOG(LogLevel::INFO) << threadName << " quit...";
            return nullptr;
        }

    public:
        ThreadPool(const int slaver_num = defaultNum)
            : _slaver_num(slaver_num), _th_sleep_num(0), _is_Running(false)
        {
            for (int i = 0; i < slaver_num; i++)
            {
                _slaver_vc.emplace_back([this]()
                                        { this->HandlerRoutine(); });
            } // 这里是让创建的线程池对象调用对应的线程函数
        }

        ~ThreadPool()
        {
        }

        void Push(T in) // 向队列输入任务 --- 思想还是生产者消费者者那一套
        {
            {
                MutexGuard lock(_mutex);
                _task_queue.push(in);
                if (_th_sleep_num > 0)
                {
                    _cond.Signal(); // 生产唤醒睡眠线程
                }
            }
        }

        /// 基本生命周期
        void Start()
        {
            _CreateThreadPool();
        }

        void Stop()
        {
            _stop();
        }

        void Wait()
        {
            _wait();
        }

    private:
        void _CreateThreadPool() // 初始化线程池意味着要创建线程
        {
            if (_is_Running)
            {
                LOG(LogLevel::WARNING) << "Thread is already running";
                return;
            }
            _is_Running = true;

            LOG(LogLevel::INFO)<<"Thread was created.....";
            for (auto &slaver : _slaver_vc)
            {
                slaver.Start();
            }
        }

        void _stop() // 这肯定是有问题的
        {
            {
                MutexGuard lock(_mutex); // 用的就是这个线程池的锁 想要整个线程停下来 必然是要所有线程进行访问这个临界资源
                _is_Running = false;
                if (_th_sleep_num > 0)
                    _cond.Broadcast();
            }
        }

        void _wait()
        {
            for (auto &slaver : _slaver_vc)
            {
                slaver.Join();
            }
        }

    private:
        std::vector<Thread> _slaver_vc; // 线程存放的容器(注意直接插入时可以用直接匿名对象来)
        int _slaver_num;                // 当前线程个数
        std::queue<T> _task_queue;      // 任务队列
        int _th_sleep_num;              // 当前线程睡眠的个数 和下面的条件变量配合使用
        Cond _cond;
        MutexMould _mutex;
        bool _is_Running;

        ////////////////////////////////
    };
};
