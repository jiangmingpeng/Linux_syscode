#include <stdio.h>

#define NON_BUFFER  1 // 1
#define LINE_BUFFER 2 // 10
#define FULL_BUFFER 4 // 000 ... 000 100
#define MODE 0666
#define MAXSIZE 1024

typedef  struct _myFILE
{
    int fd;//文件描述符
    int flags;//访问文件的方式
    int fflush_mode;//刷新缓冲区的模式
    char Outbuffer[MAXSIZE];//输出缓冲区--维护一个用来学习足以
    int pos;//写东西到缓冲区的位置
    int cap;//缓冲区的容量


}myFILE;


myFILE* myfopen(const char* pathname,const char* mode);//打开
int  myfputs(const char* s,myFILE* fp);//写入

void myfflush(myFILE* fp);//刷新缓冲区

void myfclose(myFILE* fp);//关闭文件

