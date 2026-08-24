#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <vector>

//我们的目的是模拟一个中断效果来
class task_struct
{
private:
    pid_t pid;
    int status;
    int counter;//模拟时间片 时间片是个名词 所以就用名词了

public:
    task_struct(int p):pid(p),counter(5)
    {

    }
    ~task_struct()
    {

    }
    void decs()//时间片缩减
    {
        counter--;
    }

    void reset()
    {
        counter = 5;//每次切换之后再次更新此类进程的时间片的时间 
    }
    pid_t Pid()
    {
        return pid;
    }

    bool Expired()
    {
        return (counter<=0);//时间片没有了 意思是此片刻的进程结束了
    }

    void Run()
    {
        std::cout<<"The process is running...."<<" It's pid: "<<pid<<std::endl;
    }
};
int current = 0;
std::vector<task_struct> tasks;

void do_timer(int signode)
{
    tasks[current].decs();
    if(tasks[current].Expired())
    {
        std::cout<<tasks[current].Pid()<<"过期了,请重新选择调度....."<<std::endl;
        current = rand()%tasks.size();
        tasks[current].reset();
    }
    else
    {
        tasks[current].Run();
    }
    alarm(1);
}

int main()
{
    // task_struct proc(getpid());
    // struct sigaction set;
    // set.sa_handler = do_timer;
    // struct sigaction old_set;
    // alarm(2);
    // sigaction(SIGALRM,&set,&old_set);
	alarm(1);
	signal(SIGALRM, do_timer);
	srand(time(nullptr));

	
	tasks.emplace_back(1);//因为只是模拟进程就行 这里加入后会强转
	tasks.emplace_back(2);
	tasks.emplace_back(3);
	tasks.emplace_back(4);
	tasks.emplace_back(5);
	tasks.emplace_back(7);
	tasks.emplace_back(8);
	tasks.emplace_back(9);
	tasks.emplace_back(10);
	tasks.emplace_back(11);

    for(;;)
        pause();//让当前进程主动进入休眠（阻塞）状态，直到捕获到任何一个信号为止
    return 0;
}



