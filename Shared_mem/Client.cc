#include "Shared_m.hpp"

int main()
{
    shared_mem shm;
    shm.GetShm();
    // sleep(3);

    shm.Attach();
    // sleep(3);

    char *inputbuff = (char *)shm.Addr(); // 接口内就已经判断过是否可以创建成功了 所以这里可以不用再检测
    int size = shm.Size();
    int index = 0;
    while (true)
    {
        std::cout << "Please Enter your ideas--->: ";
        std::cin >> inputbuff;
        if (strcmp(inputbuff, "quit") == 0)//这里要先写入通知Server端
            break;
        // 将 "quit" 写入共享内存，通知 Server
        // 但注意：这里写入后，Server 会读到并退出，然后 Client 也退出
        // 实际上写入后就可以 break 了，因为 Server 会自己退出
    }

    shm.UnAttach();
    sleep(3);

    return 0;
}