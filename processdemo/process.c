#include "process.h"
#define STYLE '='
#define SIZE 101



void Flushprocess(double total,double curr)
{
    if(curr>total)
        curr = total;
    double rate = curr/total*100;
    int cnt = (int)rate;//确保每达到1%时候+=一个style 模拟真实感
    static const char* lable = "|-/\\";

    char processbuff[SIZE];
    memset(processbuff,'\0',sizeof(processbuff));

    int i =0;
    for( ; i < cnt;i++ )
        processbuff[i] =STYLE;

    static int index = 0;
    printf("[%-100s][%.1lf%%][%c]\r",processbuff,rate,lable[index++]);
    index = index%strlen(lable);
    fflush(stdout);

    if(curr==total)//this out here;
        printf("\n");//下载完成进度条进行换行操作
}

void process()
{
    const char* num = "|-/\\";
    int len = strlen(num);
    char processbuff[SIZE];
    memset(processbuff, '\0', sizeof(processbuff));
    int cnt = 0;  
    while(cnt<=100)
    {
        printf("\r\033[K\033[1;33m[%-100s]\033[0m[%d%%][%c]", processbuff, cnt, num[cnt % len]);
        //printf("\033[1;33m [%-100s] \033[0m[%d%%][%c]\r",processbuff,cnt,num[cnt%len]);//这里打印是覆盖所以\r要放在后面 让后面的打印覆盖上一次的内容
        fflush(stdout);
        processbuff[cnt++] = STYLE;
        usleep(20000);
    }
    printf("\n");
}

void Count()
{
    int cnt = 10;
    while(cnt)
    {
        printf("Count :%-2d\r",cnt);
        fflush(stdout);
        sleep(1);
        cnt--;
    }
}
