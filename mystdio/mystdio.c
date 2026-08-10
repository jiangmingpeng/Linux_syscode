#include "mystdio.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


#define TRY_FFLUSH  1
#define MUST_FFLUSH 2

myFILE* myfopen(const char* pathname,const char* mode)//打开
{
    int fd = -1; 
    int flags = 0;//这个目的是用来标记文件结构体的成员变量的 实则并不影响核心功能
    if(strcmp(mode,"r") == 0)
    {
        flags = O_RDONLY;
        fd = open(pathname,O_RDONLY);

    }
    else if(strcmp(mode,"w") ==0)
    {
        flags = O_WRONLY|O_CREAT|O_TRUNC;
        fd = open(pathname,O_WRONLY|O_CREAT|O_TRUNC,MODE);

    }
    else if(strcmp(mode,"a") == 0)
    {
        flags = O_WRONLY|O_CREAT|O_APPEND;
        fd = open(pathname,flags,MODE);
    }
    else
    {
        //一些其他模式r+ a+的模式 可添加    
    }

    myFILE* fp = (myFILE*)malloc(sizeof(myFILE));

    fp->fd = fd;
    fp->flags = flags;
    fp->fflush_mode = LINE_BUFFER;
    fp->pos = 0;
    fp->cap = MAXSIZE;

    return fp;
}

static void myfflushcore(myFILE* fp,int flush)//刷新缓冲区
{
    if(fp->pos==0)
        return;
    // TRY 1 MUST 2
    if((fp->fflush_mode&LINE_BUFFER)||(flush & MUST_FFLUSH))
    {
        if((fp->Outbuffer[fp->pos-1] == '\n')||(flush & LINE_BUFFER))
        {
            write(fp->fd,fp->Outbuffer,fp->pos);
            fp->pos = 0;
        }
    }
    else if(fp->fflush_mode & FULL_BUFFER)
    {
        if(fp->pos == fp->cap)
        {
            write(fp->fd,fp->Outbuffer,fp->pos);
            fp->pos = 0;  
        }
    }
    else if(fp->fflush_mode & NON_BUFFER)
    {
        //NO
    }
    else
    {
        //NO
    }
}
int myfputs(const char* str,myFILE* fp)
{
    if(strlen(str)==0)
        return 0;
    memcpy(fp->Outbuffer+fp->pos,str,strlen(str));

    fp->pos+=strlen(str);

    myfflushcore(fp,TRY_FFLUSH);//这里应该是尝试刷新的 因为可以检查是否符合规定刷新条件

    return strlen(str);
}

void myfflush(myFILE* fp)
{
    myfflushcore(fp,MUST_FFLUSH);//调用ffush那肯定是需要刷新 所以参数没毛病
}

void myfclose(myFILE* fp)//关闭文件
{
    //强制刷新
    myfflush(fp);

    //强制刷新磁盘
    //fsync(fp->fd); 可选

    //关闭文件
    close(fp->fd);

    //释放空间
    free(fp);

}
