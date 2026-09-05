#pragma once
#include <iostream>
#include <functional>
#include <vector>

using tasks_t = std::function<void()>;//用这个思路做任务管理感觉挺不错的


void Print()
{
    std::cout<<"正常执行打印任务 Print"<<std::endl;
}

void Load()
{
    std::cout<<"正常执行下载任务 Load"<<std::endl;
}

void Upload()
{
    std::cout<<"正常执行上传任务 Upload"<<std::endl;
}
void Memory()
{
    std::cout<<"正常执行存储任务 Memory"<<std::endl;
}
void Visit()
{
    std::cout<<"正常执行访问任务 Visit"<<std::endl;
}
std::vector<tasks_t> TaskList()
{
    std::vector<tasks_t> tasks;
    tasks.emplace_back(Print);
    tasks.emplace_back(Load);
    tasks.emplace_back(Upload);
    tasks.emplace_back(Memory);
    tasks.emplace_back(Visit);

    return tasks;
}
