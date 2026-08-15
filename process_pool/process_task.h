#pragma once
using task = std::function<void()>;


void DoTask(int fd);
///////////////////////////////子进程要完成的任务/////////////////////////
void SyncDisk();

void Download();

void Upload();

void PrintLog();

void UpdateStatus();

// 声明初始化函数
void InitTasks(std::vector<task>& list);