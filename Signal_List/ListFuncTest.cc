#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void PrintStatus(sigset_t& set)
{
    std::cout << "curr process[" << getpid() << "]pending: ";
    for (int i = 31; i > 0; i--)
    {
        if(sigismember(&set,i))
        {
            std::cout<<"1";
        }
        else
        {
            std::cout<<"0";
        }
    }
    std::cout<<std::endl;
}

void handler(int signode)//验证取消屏蔽(卡在阻塞到递达的过程)是在捕获前还是在捕获后
{
    std::cout << "signode: "<<signode<<std::endl;
    sigset_t sigset;
    sigemptyset(&sigset);
    sigpending(&sigset);
    PrintStatus(sigset);
    std::cout<<"---------------------------------"<<std::endl;
}//由运行事实得知 取消屏蔽是在捕获之前 因为捕获中的pending表已经调回0了 意味着屏蔽取消在进入捕获函数之前就已经完成了

int main()
{

    // 1.创建新的信号集,并且屏蔽2号信号
    // 捕获一下2号信号的默认行为
    signal(2, handler);
    sigset_t block_set;
    sigset_t old_set;
    sigemptyset(&block_set);
    sigemptyset(&old_set);
    // 把2号信号添加到信号集里
    sigaddset(&block_set,2);
    sigprocmask(SIG_BLOCK, &block_set, &old_set); // 利用上面的信号集屏蔽这个2号
    // 到这里意味着此时block信号集里2是1  所以这里就无法信号递达
    // 前面操作的是block表 现在操作pending表
    int cnt = 12;
    while(true)
    {
        //2.获取当前信号集
        sigset_t pending;
        sigemptyset(&pending);
        sigpending(&pending);//我就知道这个表不能凭空改变 肯定要有能访问真正的内核的那张表来输出 到空白信号集
        //3.打印pending表
        PrintStatus(pending);//我们要打印的是pending表 这里传的是空信号集 按道理来讲确实是打印全0
        //4.解除对2的屏蔽
        if(cnt == 0)
        {
            std::cout<<"解除2号信号的屏蔽 回到000"<<std::endl;
            sigprocmask(SIG_SETMASK,&old_set,nullptr);
        }
        cnt--;
        sleep(1);
    }

    return 0;
}