#include "mystdio.h"
#include <string.h>
#include <unistd.h>


int main()
{
    myFILE *fp = myfopen("log.txt", "a");
    if(fp == NULL)
    {
        printf("myfopen error!\n");
        return 0;
    }
    
    const char *msg = "hello jmp\n";
    int cnt = 10;
    while(cnt--)
    {
        myfputs(msg, fp);
        sleep(1);
        printf("debug: outbuffer = %s, pos = %d\n", fp->Outbuffer, fp->pos);
    }
    myfclose(fp);
    printf("write file done!\n");
        
//    mvyFILE* fp = myfopen("log.txt","w");
//    if(NULL==fp)
//    {
//        perror("myfopen");
//        return -1;
//    }
//
//    const char* str = "hello jmp";
//    int cnt = 10;
//    while(cnt--)
//        myfputs(str,fp),sleep(1),cnt--;
//
//    myfclose(fp);

    return 0;
}
