#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <cstdlib>
int main(int argc,char* argv[])
{       

    if(argc != 3)
    {
        std::cout<<"Usage: "<<"./mykill -signalcode pid" <<std::endl;
        exit(1);
    }

    int signalcode = std::stoi(argv[1]+1);
    pid_t pid = std::stoi(argv[2]);
    int n = kill(pid,signalcode);
    (void)n;

    return 0;
}