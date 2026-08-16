#include "named_pipe.hpp"

int main()
{
    std::cout<<"Client"<<std::endl;
    pipefile client;
    client.GetPipeFile(ForWrite);
    
    std::string msg;

    std::cout<<"----------退出输入^quit^---------"<<std::endl;
    while (true)
    {
        std::cout<<"Please input your ideas:-->";
        std::cin>>msg;
        if(!client.Send(msg))
            break;
    }
    return 0;
}