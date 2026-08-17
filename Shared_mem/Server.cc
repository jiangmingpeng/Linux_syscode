#include "Shared_m.hpp"

int main()
{
    shared_mem shm;
    shm.CreatShm();
    // sleep(3);

    shm.Attach();
    // sleep(3);

    char *outputbuff = (char *)shm.Addr(); // 接口内就已经判断过是否可以创建成功了 所以这里可以不用再检测
    int size = shm.Size();

    while (true)
    {
        if (strcmp(outputbuff, "quit") == 0)
        {
            std::cout << "Received quit signal, exiting..." << std::endl;
            break;
        }//这里要先判断 响应客户端
        // 本质就是读取共享内存
        // 临界区代码
        std::cout << outputbuff << ' ';
        std::cout << std::endl;
        sleep(1);
    }

    shm.UnAttach();
    sleep(3);

    shm.Delete();

    return 0;
}