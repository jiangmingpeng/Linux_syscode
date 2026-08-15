#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <functional>
#include "process_task.h"

/////////////////进程任务声明////////////////
extern std::vector<task> Task_list;
using cb_t = std::function<void(int)>;
/////////////////进程池相关/////////////////
#define ProcessNum 10//不得不说 我感觉自己这个解耦性还是可以的
static const int gprocess = ProcessNum; // 声明为静态全局变量+const修饰 既保证了不被重复定义 又避免被意外修改
enum
{
    OK = 1, // 可以规定枚举常量从哪里开始 以便于自己维护退出码
    PIPE_ERRO,
    FORK_ERRO
};

class Process_pool
{
    class Channel // 定义一个内部类 直接描述一个管道 并且对应一个进程的情况
    {
    public:
        Channel(int pid, int wfd) : _wfd(wfd), _subpid(pid)
        {
            _subname = "sub-channel-" + std::to_string(_subpid); // 命名空间没有展开 别忘了
        }
        ~Channel()
        {
        }
        std::string Name()
        {
            return _subname;
        }
        void PrintInfo()
        {
            std::cout << "wfd:" << _wfd << " _subname:" << _subname << std::endl;
        }
        void Write(int itask)
        {
            write(_wfd,&itask,sizeof(itask));
            
        }
        void Close()
        {
            close(_wfd);
            std::cout<<"管道已关闭 "<<std::endl;
        }
        void Wait()
        {
            int status = 0;
            pid_t rid = waitpid(_subpid,&status,0);
            if(rid<0)
            {
                perror("waitpid:");
            }
            std::cout<<"进程已回收 "<<std::endl;
        } 
private:
        int _wfd;             // 写端开的fd
        int _subpid;          // 所对应创建的子进程
        std::string _subname; // 所对应创建子进程的名字
    };

public:
    void Init(cb_t cb)
    {
        CreateProcess_pool(cb);
    }
    void Debug() // 查看创建的进程数量的具体信息
    {
        for (auto &e : channels)
        {
            e.PrintInfo();
        }
    }
    void Run(int count = 999999)//程序跑起来也不能 一直跑 但是不回收子进程会变成僵尸进程 所以可以+个参数来表示用户层的执行意思
    {
        while (!_quit)
        {
            std::cout << "---------------------------------------------------" << std::endl;
            // 1.轮询选择(管道+进程)
            int iprocess = SelectProcess();
            std::cout << "选择的进程是: " << channels[iprocess].Name().c_str() << std::endl;
            // 2.随机选择任务
            int itask = SelectTask();
            std::cout << "此进程选择的任务是: " << itask << std::endl;
            // 3.发送任务到(管道+进程)
            std::cout<<"正在把任务发送给:"<<channels[iprocess].Name().c_str()<<std::endl;
            SendTask2Process(itask,iprocess);
            std::cout << "任务发送成功!!! " << std::endl;
            // sleep(1);实际开发不用sleep 影响性能
            if(count > 0 &&--count == 0)    break;//防止传0也进入任务
        }

    }

    void Stop()
    {
        _quit = true;
    }
    
    void Quit()
    {
        //version2
        for(auto& channel:channels)
        {
            channel.Close();
            channel.Wait();
        }
        //version2(没有优化指针计数器之前)
        //逆向回收---这下就不会被前面的多指针所影响 因为最后一个总是一条线 最后一个关了 那么前一个也就是最后一个了 以此类推
        // int end = channels.size()-1;
        // while(end >= 0)
        // {
        //     channels[end].Close();
        //     channels[end].Wait();
        //     end--;
        // }
        //version1先关管道close再关进程waitpid
        // for(auto& channel:channels)
        // {
        //     channel.Close();
        // }

        // for(auto& channel:channels)
        // {
        //     channel.Wait();
        // }
    }
private:
    void CreateProcess_pool(cb_t cb)
    {
        srand((unsigned int)time(NULL));
        for (int i = 0; i < gprocess; i++) // 创建进程池
        {
            int pipefd[2] = {0};
            int n = pipe(pipefd);
            if (n < 0)
            {
                std::cerr << "pipe()" << std::endl;
                exit(PIPE_ERRO);
            }
            pid_t pid = fork();
            if (pid < 0)
            {
                std::cerr << "fork()" << std::endl;
                exit(FORK_ERRO);
            }
            else if (pid == 0) // 子进程---读取数据以找到要完成的任务
            {
                // 子进程关闭历史wfd， 影响的是自己的fd表
                if(!channels.empty())
                {
                    for(auto& channel:channels)
                        channel.Close();//其实就是关了创建出来之后的多余的
                }
                close(pipefd[1]);
                cb(pipefd[0]); // 传的是读端文件描述符
                exit(OK);
            }
            else // 父进程 --这个父进程的代码可以不写在else里面
            {
                close(pipefd[0]);
                channels.emplace_back(Channel(pid, pipefd[1])); // 写内容让程序跑起来可以 单独写 而不是一创建就跑
                std::cout << "创建子进程:" << pid << " 成功...." << std::endl;
                // sleep(1);
            }
        }
    }

    int SelectProcess()
    {
        static int index = 0;
        int selected = index;//用中间量来记录中间的静态变量 可以更清楚的展示过程
        index++;
        index %= channels.size();
        return selected;
    }
    int SelectTask()
    {
        // 把 rand() 的返回值向右移几位（抛弃掉低位，用高位）
        // 然后再 % 5，处理 rand 最低位不随机的经典做法
        return (rand() >> 4) % 5;
    }
    void SendTask2Process(int itask,int iprocess)
    {
        // int num =Task_list.size();
        // if(itask>= num||itask < 0)-----估计是这个size可能出了 问题但是目前的调试无法快速建立 暂时先不看这里
        if(itask < 0)
            return;
        if(iprocess<0||iprocess>=channels.size())
            return;
        channels[iprocess].Write(itask);
        // write(channels[iprocess]._wfd,&itask,sizeof(itask));
    }

private:
    std::vector<Channel> channels;
    bool _quit = false;
};
