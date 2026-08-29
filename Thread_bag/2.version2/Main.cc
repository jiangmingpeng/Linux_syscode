#include "Thread.hpp"
#include <unistd.h>
#include <vector>


#define Type Data 
class Data
{
public:
    Data(int a = 0,int b = 0,int c = 0,int d = 0):_a(a),_b(b),_c(c),_d(d)
    {}
    ~Data(){}
    //方法 其实更重要的是下面的参数 因为这个类对象传入函数 是用来给函数内部来使用这个类的
    std::string Add()
    {
        return std::to_string(_a)+"+"+std::to_string(_b)+"="+std::to_string(_a+_b);
    }
// private://参数
    int _a;
    int _b;
    int _c;
    int _d;
};


void *Routine(Data& set)//类类型用引用减少消耗 提高效率 //具体参数个数由上面的类来给出 然后由里面回调的时候传给给它
{
    while (true)
    {
        std::cout << "正在执行业务任务..." << std::endl;
        std::cout << "you are right..." << std::endl;
        std::cout << "yes i'm fine..." << std::endl;
        std::cout <<"a:"<< set._a<<" b:"<<set._b<< std::endl;
        std::cout <<"c:"<<set._c<<" d:"<<set._d<< std::endl;
        std::cout<<set.Add()<<std::endl;
        sleep(1);
    }
    return nullptr;
}

int main()
{
    Type td(10,20);//参数的具体值则由用户 手动创建对象来传入内部线程类来进行初始化线程内部的初始化
    std::vector<ThreadMould::Thread<Type>> threads; //表示这个模板实例化的初始值
    int threadNum = 0;
    std::cin>>threadNum;
    for(int i = 0;i<threadNum;i++)
    {
        threads.emplace_back(Routine,td);//这里能传两个参数得益于里面的线程类 构造函数里有两个参数
    }
    for (auto &t : threads)
    {
        t.Start();
        // sleep(1);
    }

    sleep(10);
    for (auto &t : threads)
    {
        t.Die();
    }

    for (auto &t : threads)
    {
        t.Join();
        t.PrintInfo();
        // sleep(1);
    }
    return 0;
}