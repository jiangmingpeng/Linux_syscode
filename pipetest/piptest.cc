#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstdio>
#include <cstring>

int main()
{

    int pipefd[2] = {0};//建立管道
    int n = pipe(pipefd);
    if(n<0)
    {
        perror("pipe");
    }
    pid_t pid = fork();
    if(pid<0)
    {
        perror("fork");
    }
    const char* msg = "send msg to parent....";
    char inputbuff[1024];
    if(pid == 0)//child
    {
        close(pipefd[0]);
        int cnt = 5;
        int count = 0;
        while(cnt)
        // while(1)
        {
            snprintf(inputbuff,sizeof(inputbuff),"%s---%d\n",msg,cnt--);
            write(pipefd[1],inputbuff,sizeof(inputbuff)-1);
            sleep(1);
            // count++;
            // std::cout<<count<<std::endl;
        }
        exit(1);
    }
    //parent

    char outputbuff[1024];
    close(pipefd[1]);
    close(pipefd[0]);
    // int cnt = 10;
    // while(cnt--)
    // // while(1)
    // {
    //     ssize_t n = read(pipefd[0],outputbuff,sizeof(outputbuff)-1);//留一个位置用来放 \0
    //     std::cout << "n=" << n << std::endl;
    //     if(n<=0)     break;//有无读到 读到多少需要记录下来 为后面的判断做铺垫
    //     outputbuff[n] = '\0';
    //     std::cout<<outputbuff<<std::endl;
    //     sleep(1);
    // }
    int status = 0;
    pid_t rid = waitpid(pid,&status,0);
    std::cout<<"rid:"<<rid <<" "<<"exit_code:"<<((status>>8)&0xFF)<<" sig_code:"<<(status&0x7F)<<std::endl;

}