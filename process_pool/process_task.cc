#include "process_pool.h"//多个cc文件包含同一个头文件时 可能出现类型重定义的现象 因为头文件在预处理时 被重复展开了 可以把头文件的全局变量 通过extern 表示向外声明的 在cc 文件中来定义
/////////////////进程任务声明////////////////
std::vector<task> Task_list;
void DoTask(int fd)
{
    InitTasks(Task_list); // 把任务列表初始化一下 后续就可以根据负载均衡的模式来设计 控制进程的方式
    while (true)//准备开始进入任务模式 等待父进程写入 写入之前子进程处于阻塞状态--这个状态再看
    {
        int task_code = 0;
        ssize_t n = read(fd, &task_code, sizeof(task_code)); // 这里用的是整数退出码的形式
        if (n == sizeof(task_code))//因为用的int码 所以也正常 既然读到就可能是要执行的任务码
        {
            // 再根据读到的任务码进行工作安排
            if(task_code<Task_list.size()&&task_code >= 0)//注意这里看的下标
            {
                Task_list[task_code]();
            }
        }
        else if (n == 0)//啥也没读到 意味父进程不在写入  其实就是没有写的时候 把运行单独放一个模块就可以实现 按钮式的开启关闭
        {
            std::cout<<getpid()<<" task quit ......"<<std::endl;
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }
}
///////////////////////////////子进程要完成的任务/////////////////////////
void SyncDisk()
{
    std::cout << getpid() << ": 刷新数据到磁盘任务" << std::endl;
    // sleep(1);
}

void Download()
{
    std::cout << getpid() << ": 下载数据到系统中" << std::endl;
    // sleep(1);
}

void Upload()
{
    std::cout<<getpid() << ": 上传数据到云端中" << std::endl;
    //sleep(1);
}

void PrintLog()
{
    std::cout << getpid() << ": 打印日志到本地" << std::endl;
    // sleep(1);
}

void UpdateStatus()
{
    std::cout << getpid() << ": 更新一次用户的状态" << std::endl;
    // sleep(1);
}




// 实现初始化函数：批量注册任务
void InitTasks(std::vector<task>& list) 
{
    list.emplace_back(SyncDisk); // 填入函数名
    list.emplace_back(Download);
    list.emplace_back(Upload);
    list.emplace_back(PrintLog);
    list.emplace_back(UpdateStatus);
}

