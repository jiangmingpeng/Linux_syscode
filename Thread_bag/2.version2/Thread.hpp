#pragma once
#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include <cstring>


namespace ThreadMould
{
    static int gnumber = 1; // 记录第几个线程的变量
 
    template<typename T>
    using callback_t = std::function<void(T &)>; 
    enum STATUS
    {
        THREAD_NEW,
        THREAD_RUNNING,
        THREAD_STOP
    };
    //-------------
    std::string Status2String(STATUS s)
    {
        switch (s)
        {
        case STATUS::THREAD_NEW:
            return "THREAD_NEW";
        case STATUS::THREAD_RUNNING:
            return "THREAD_RUNNING";
        case STATUS::THREAD_STOP:
            return "THREAD_STOP";
        default:
            return "UNKNOWN";
        }
    }
    std::string IsJoined(bool joinable)
    {
        return joinable ? "true" : "false";
    }
    //--------------上面的繁琐直接复制了


    template<class T>
    class Thread
    {
    public:
        // 初始化一个线程类 传入要调的函数
        Thread(callback_t<T> cb,const T& data) : _tid(-1), _status(THREAD_NEW), _cb(cb), _joinable(true),_data(data)
        {
            _name = "New-Thread-" + std::to_string(gnumber++); // 明明可以直接赋值给_name 还去折腾那破函数
        }
        ~Thread()
        {
        }

        bool Start()
        {
            int n = pthread_create(&_tid, nullptr, Routine, this); // 虽然这里不用c_str()但是也要知道这个把string转str的函数 然后再方便转其他类型
            if (n != 0)                                            // 代码健壮性
            {
                return false;
            }
            ToRunning();
            return true;
        }

        void Die()
        {
            if (_status == STATUS::THREAD_RUNNING)
            {
                int n = pthread_cancel(_tid);
                if(n == 0)
                    ToStop();
                else
                    std::cerr<<"pthread_cancel failed: "<<n<<std::endl;
            }
        }

        void Detach()
        {
            if (_status == THREAD_RUNNING && _joinable)
            {
                pthread_detach(_tid);
                _joinable = false;
            }
            else
                std::cout << "Datach fail...." << std::endl;
        }

        void Join()
        {
            if (_joinable)
            {
                int n = pthread_join(_tid, &result);
                if (n != 0)
                {
                    std::cerr << "join error: " << n << std::endl;
                    return;
                }
                ToStop();
            }
            else
            {
                std::cerr << "error, thread join status: " << IsJoined(_joinable) << std::endl;
            }
        }

        void PrintInfo()
        {
            std::cout << "thread name : " << _name << std::endl;
            std::cout << "thread _tid : " << _tid << std::endl;
            std::cout << "thread _status : " << Status2String(_status) << std::endl;
            std::cout << "thread _joinable : " << IsJoined(_joinable) << std::endl;
        }

    private:
        // 不能在参数列表里写上 this我 们发现对于pthread_create();无法传过去 其原因是类中方法第一个参数默认是this指针
        //  void* Routine(Thread* const this,void* args)
        static void *Routine(void *self)
        {
            Thread<T> *Self = static_cast<Thread<T> *>(self); // 强转类型效果只有一行 要想在域内使用 可以转成目标类型后保存给后续使用
            pthread_setname_np(Self->_tid, Self->_name.c_str());
            Self->_tid = pthread_self();
            Self->_cb(Self->_data);//唯一感觉有点技术含量的地方 这个回调和这个this指针的运用 //在外面设置参数 在里面初始化参数 再在里面传参数
            Self->ToStop();
            return nullptr;
        }
        void ToStop()
        {
            _status = STATUS::THREAD_STOP;
        }
        void ToRunning()
        {
            _status = STATUS::THREAD_RUNNING;
        }

    private:
        std::string _name;
        pthread_t _tid; // 注意这个线程库的写法
        STATUS _status;
        callback_t<T> _cb;
        bool _joinable;
        void *result;
        //调用传参函数
        T _data;
    };
}
