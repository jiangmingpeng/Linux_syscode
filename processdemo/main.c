#include "process.h"
#include <time.h>
#include <stdlib.h>

double total = 1024.0;
double speed = 3.2;

typedef void (*callback_t)(double,double);

double NetSpeed(double _start,double _range)
{
   double fraction = rand()/(double)RAND_MAX;//生成0-1之间的浮点数
   return _start + fraction*_range;
}

void DownLoad(int total,callback_t cb)
{
    double curr = 0;

    while(1)
    {
       if(curr>total)
       {
           curr = total;
           cb(total,curr);
           break;
       }
       cb(total,curr);
       curr+=NetSpeed(speed,37.4);
       usleep(37863);//设置大概多久跑完进度条应该在这里设置的
    }
}



int main()
{
    srand(time(NULL));
    //process();
    //Count();
    //DownLoad(total,Flushprocess);//这里传了个函数指针
    printf("download: 20.0MB\n");
    DownLoad(20.0, Flushprocess);
    printf("download: 2000.0MB\n");
    DownLoad(2000.0, Flushprocess);
    printf("download: 100.0MB\n");
    DownLoad(100.0, Flushprocess);
    printf("download: 20000.0MB\n");
    DownLoad(20000.0, Flushprocess);
    return 0;
}
