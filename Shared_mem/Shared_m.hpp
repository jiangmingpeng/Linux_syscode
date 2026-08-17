#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <cstdio>
#include <cerrno>
#include <cstring>

const char *pathname = "/tmp";
int proj_id = 0x12;
// const size_t ShmSize = 4096;
const size_t ShmSize = 128;

enum
{
    OK = 1,
    ATTACH_ERR,
    K_ERR,
    CR_ERR,
    SHDT_ERR,
    PRI_ERR
};

class shared_mem
{
public:
    shared_mem()
    :_shmid(-1),_size(ShmSize),_address(nullptr)
    {

    }
    ~shared_mem()
    {

    }
    // 1.创建共享内存
    void CreatShm()
    {
        _ShmApply(IPC_CREAT| IPC_EXCL | 0666);

    }
    // 2.获取共享内存
    void GetShm()
    {
        _ShmApply(IPC_CREAT);//权限位在创建的时候就已经设置了
    }
    // 3.挂载共享内存--把共享区的内存映射到进程地址上去
    void Attach()
    {
        //挂载到OS分配的进程地址上 第二个参数的意思
        _address = shmat(_shmid,nullptr,0);//现在映射到的是一块没有类型的地址上--这里再看看
        if(_address == (void*)-1)
        {
            std::cerr<<"shmat: "<<strerror(errno)<<std::endl;
            exit(ATTACH_ERR);
        }
    }
    // 4.取消挂接共享内存
    void UnAttach()
    {
        int n = shmdt(_address);
        if(n < 0)
        {
            std::cerr<<"shmdt: "<<strerror(errno)<<std::endl;
            exit(SHDT_ERR);
        }
    }
    // 5.删除共享内存
    void Delete()
    {
        int n = shmctl(_shmid,IPC_RMID,nullptr);//删除当前shmid所对应的共享内存__IPC_RMID=0
        (void)n;
        std::cout<<"file was delete.."<<std::endl;
    }

    //6.附加接口
    void* Addr()
    {
        return _address;
    }

    void PrintAttr()//打印当前共享内存的基本属性
    {
        struct shmid_ds ds;
        int n = shmctl(_shmid, IPC_STAT, &ds);
        if(n < 0)
        {
            perror("shmctl");
            exit(PRI_ERR);
        }
        printf("key: 0x%x\n", ds.shm_perm.__key);
        printf("shm_nattch: %ld\n", ds.shm_nattch);
        printf("shm_segsz: 0x%lx\n", ds.shm_segsz);
    }
    int Size()
    {
        return _size;
    }
private:
    //创建或者获取共享内存
    void _ShmApply(int flags)
    {
        // 随机生成一个key 来充当唯一标识符
        key_t k = ftok(pathname, proj_id);
        if (k < 0)
        {
            std::cerr << "ftok :" << strerror(errno) << std::endl;
            exit(K_ERR);
        }
        // 创建共享内存
        _shmid = shmget(k, ShmSize, flags); // k可以自己写 最佳实践是调用ftok
        if (_shmid < 0)
        {
            std::cerr << "shmget: " << strerror(errno) << std::endl;
            exit(CR_ERR);//出错的地方应该直接退出的
        }
        // Debug
        printf("key:0x%x\n", k);
    }
private:

    int _shmid;
    int _size;
    void* _address;//共享内存的其实内存地址

};
