#include "Thread.hpp"
#include "Tasks.hpp"
#include "Logger.hpp"
#include "ThreadPool.hpp"
#include <ctime>
#include <unistd.h>

using namespace ThreadPoolMould;

int main()
{
    auto tasks = TaskList();
    srand(time(nullptr) ^ getpid());
    ThreadPool<tasks_t> tp;

    tp.Start();

    while (true)
    {
        int index = rand() % tasks.size();
        usleep(137);
        tp.Push(tasks[index]);
        sleep(1);
    }

    sleep(5);
    tp.Stop();
    tp.Wait();

    return 0;
}