#include "named_pipe.hpp"

int main()
{
    std::cout<<"Server"<<std::endl;
    pipefile server;
    server.CreatePipeFile();
    server.GetPipeFile(ForRead);
    std::string msg;

    while (true)
    {
        int n = server.Recv(&msg);
        if(n > 0)
            std::cout << "Client Say-->: " << msg << std::endl;
        else
            break;
    }
    server.DeletePipefile();
    return 0;
}