#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

enum
{
    OK = 1,
    MKFIFO_ERRO,
    OPEN_ERRO
};

#define ForRead  1//这个宏用来 获取文件时的方式
#define ForWrite 2
const char* gcomment = "./fifo"; //这里对应的为mkfifo的第一个参数 在什么路径下创建什么管道文件 (const char* _pathname)

class pipefile//管道文件这个对象 只要描述一次即可 后面在不同进程上 创建出来的都是分别对应自己这个进程的 对应文件描述符
{
public:
    pipefile(const std::string& g_comment = gcomment)
    :_gcomment(g_comment),_fd(-1),_mode(0666)
    {

    }
    ~pipefile()
    {

    }
    //1.创建管道文件
    void CreatePipeFile()
    {
        umask(0);//手动设置一下掩码 保证权限位根据自己的需求来
        //创建新的管道文件之前 先要判断是否已经存在此文件
        if(IsExit())//存在应该是删掉重建 而不是使用当前存在的 因为并不能完全确认就是管道文件 创建的性能消耗基本忽略不计
        {
            unlink(_gcomment.c_str());
        }
        int n = mkfifo(gcomment,_mode);
        if(n<0)
        {
            std::cerr<<"mkfifo fail: "<<strerror(errno)<<" errno:"<<errno<<std::endl;
            exit(MKFIFO_ERRO);
        }
        std::cerr<<"mkfifo success: "<<strerror(errno)<<" errno:"<<errno<<std::endl;
        std::cout << "Server waiting for client to connect..." << std::endl;
    }
    //2.获取管道文件
    void GetPipeFile(int flags)
    {
        //所谓获取管道文件 其实就是用打开的方式
        if(flags == ForRead)
        {
            _fd = open(_gcomment.c_str(),O_RDONLY);
        }
        else if(flags == ForWrite)
        {
            _fd = open(_gcomment.c_str(),O_WRONLY);//每个pipefile都对应着自己的_gcomment 全局只是用来标识的 否则有问题
        }
        else
        {
            std::cout<< "Please input correct ways..."<<std::endl;
            return;
        }
        //打开成功与否
        if(_fd < 0)
        {
            std::cerr << "open error: " << strerror(errno) << " errno: " << errno << std::endl;
            exit(OPEN_ERRO);
        }
        else
        {
            std::cout << "open file success" << std::endl;
        }
    }
    //3.获取管道文件后通信的具体事件
    bool Send(const std::string& smsg)
    {
        Quit(smsg);
        if(_Run == true)
        {
            _send(smsg);
            return true;
        }
        else
        {
            std::cout<<"task exited normally.."<<std::endl;
            return false;
        }
    }
    int Recv(std::string* rmsg)
    {
        return _recv(rmsg);
    }
    //4.退出通信
    void Quit(const std::string& str)
    {   
        if(str == "quit")
            _quit();
        return;
    }
    //5.删除管道文件
    void DeletePipefile()
    {
        //删除之前也要判断文件是否存在
        if(!IsExit())
        {
            return;
        }
        unlink(_gcomment.c_str());//能走到这里那么肯定是此文件存在 参数别搞错了 用的string 参数要求str的记得改
        std::cout<<"Already delete pipefile.."<<std::endl;
    }
private:
    bool IsExit()
    {
        struct stat statbuff;
        int n = stat(_gcomment.c_str(),&statbuff);//查看某个路径下的文件是否存在
        if(n<0)
        {
            std::cout<<"file not exit.."<<std::endl;
            errno = 0;
            return false;
        }
        else if(n==0)
        {
            std::cout<<"file exit.."<<std::endl;
            errno = 0;
            return true;
        }
        else{}
    }
    void _send(const std::string& smsg)//如果是信息交流:其实就是把msg写入管道文件缓冲区里
    {
        ssize_t n = write(_fd,smsg.c_str(),smsg.size());
        (void)n;//表示不用此变量
    }
    int _recv(std::string* rmsg)//输出型参数
    {
        char outbuff[512];
        ssize_t n = read(_fd,outbuff,sizeof(outbuff)-1); //系统调用是不会置0的 所以最后一位要手动置0 防止读到乱码
        if(n == 0)
        {
            return 0;
        }
        else if(n>0)
        {
            outbuff[n] = '\0';
            *rmsg = outbuff;
            return n;
        }
        else
        {
            return -1;
        }
    }
    void _quit()
    {
        _Run = false;
        close(_fd);
    }
private:
    const std::string _gcomment;
    int _fd;//维护一个当前进程打开所对应的文件描述符
    mode_t _mode;//维护一个创建管道文件的权限码 注意可能受到掩码的影响没有达到目标权限位 可以手动置umask为0
    bool _Run = true;//维护一个是否要退出服务的状态码

};
