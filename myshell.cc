#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

#define MAXSIZE 128
#define MAXARGS 32



//表明重定向信息
#define NoneRedir   0
#define InputRedir  1
#define AppRedir    2
#define OutputRedir 3
#define Trimspace(start) do{ while(isspace(*start))  start++;  }while(0)
int  redir_type = NoneRedir;//记录正在执行的执行，重定向方式
char* filename = NULL;//保存重定向的目标文件


//维护一张全局环境变量表
char* genv[MAXARGS];
int genvc = 0;

//维护一张命令行参数表
char* gargv[MAXARGS];
int gargc = 0;
const char* gsep = " ";

//维护一个全局退出码 每次执行新的命令 就要更新退出码
int lastcode = 0;

//shell自己所处的工作路径
char cwd[MAXSIZE];

void LoadEnv()
{
    //这份环境变量表是从 父进程bash里拷贝过来 维护的一张全局表
    extern char** environ;
    for(int i = 1;environ[i];i++)
    {
        genv[i]=environ[i];
        genvc++;
        cout<<genv[i]<<endl;
    }

}

const char* GetUserName()
{
    char* user = getenv("USER");
    if(NULL==user)
        return "None";
    return user;
}

const char* GetHostName()
{
    char* host = getenv("HOSTNAME");
    if(NULL==host)
        return "None";
    return host;
}

const char* GetPwd()
{
    char* pwd = getenv("PWD");
    if(NULL==pwd)
        return "None";
    return pwd;
}

string SubPwd()
{
    string pwd = GetPwd();
    if(pwd == "/")
    {
        return "/";
    }
    auto pos = pwd.rfind('/');
    if((pos == string::npos))
        return string();
    return string(pwd,pos+1).c_str();
}
void PrintCommandLine()//对于最后打印路径  按道理来讲只要 打印最后所在的文件路径就可以
{
    //printf("[%s@%s %s]#",GetUserName(),GetHostName(),SubPwd());
    cout<<"["<<GetUserName()<<"@"<<GetHostName()<<" "<<SubPwd()<<"]"<<"#";
    fflush(stdout);
}
int GetCommandLine(char* commandline,int size)
{
    if(NULL==fgets(commandline,size,stdin))
    {
        return 0;
    }

    commandline[strlen(commandline)-1] = '\0';//把用户手动敲的\n换成结束符
    return strlen(commandline);
}

int SepCommandLine(char* commandline)
{
    gargc = 0;
    memset(gargv,0,sizeof(gargv));//每次执行新的命令 就要初始化命令行参数
    gargv[gargc] = strtok(commandline,gsep);
    while((gargv[++gargc] = strtok(NULL,gsep)));
    return gargc;
    //for(int i = 0;gargv[i];i++)
    //{
    //    cout<<gargv[i]<<" ";
    //}
    //cout<<endl;
}
//"ls -a -l >> log.txt"--> "ls -a -l"\0\0 "log.txt"||"ls -a -l > log.txt"-->"ls -a -l"\0"log.txt"|| < 也是一样的道理 目的就是成这样
void ParseRedir(char* commandline)
{
    char* start= commandline;
    char* end = commandline+strlen(commandline);//左闭右开

    while(start<end)
    {
        if(*start=='>')
        {
            if(*(start+1)=='>')
            {
                *start = '\0';
                start++;
                *start = '\0';
                start++;
                Trimspace(start);
                filename = start;
                redir_type = AppRedir;
                break;
            }
            *start = '\0';
            start++;
            Trimspace(start);
            filename = start;
            redir_type = OutputRedir;
            break;
        }
        else if(*start == '<')
        {
            *start = '\0';
            start++;//这里要++的地方 不要漏了 否则指向的字符串不对劲
            Trimspace(start);
            filename = start;
            redir_type = InputRedir;
            break;
        }
        else 
        {
            start++;
            filename = NULL;
            redir_type = NoneRedir;
        }
    }
}
int StartCommandLine()
{
    int status = 0;
    pid_t pid = fork();

    if(pid<0)
    {
        return -1;
    }
    else if(0==pid)
    {  
        int fd = -1;
        if(redir_type == InputRedir)
        {
            fd = open(filename,O_RDONLY);//只读 既然要改变从键盘上获取数据的途径 那么这个地方就得有内容 
            dup2(fd,0);
        }
        else if(redir_type == OutputRedir)
        {
            fd = open(filename,O_WRONLY | O_CREAT | O_TRUNC,0666);//既然要写东西 有就从0写 没有就创建文件
            dup2(fd,1);
        }
        else if(redir_type == AppRedir)
        {
            fd = open(filename, O_WRONLY | O_CREAT | O_APPEND,0666);//对标上面
            dup2(fd,1);
        }
        else if(redir_type == NoneRedir)
        {
            //不干嘛
        }
        else 
        {
            //无内容
        }//进程替换 并不会影响文件的打开和重定向
        execvp(gargv[0],gargv);//对于需要让父进程执行的命令才能达到理想效果的比如"cd" 这些称为内建命令 如果让子进程执行 马上就退出了 所以看不到理想的效果
        //execvpe(gargv[0],gargv,genv);//对于需要让父进程执行的命令才能达到理想效果的比如"cd" 这些称为内建命令 如果让子进程执行 马上就退出了 所以看不到理想的效果
        exit(1);
    }
    else
    {
        pid_t rid = waitpid(-1,&status,0);
        if(rid>0)
        {
            lastcode = WEXITSTATUS(status); //每次正常执行完指令 都有一个退出码 自然也就在这里更新               
        }
    }
    return 0;
}

int ExcuteBuiltInCommandLine()
{
    if(strcmp(gargv[0],"cd")==0)//这个等于0 是相等的返回值 
    {
       if(gargc == 2)
       {

           //1.更改进程内核中的路径
           chdir(gargv[1]);//既然用的cd那后面第一个 参数肯定是路径 所以路径改 成gargv[1]就可以
           //2.更改环境变量
           char pwd[1024];
           getcwd(pwd,sizeof(pwd));
           snprintf(cwd,sizeof(cwd),"PWD=%s",pwd);
           putenv(cwd);
           lastcode = 0;
       }
       return 1;
    }
    else if(0 == strcmp(gargv[0],"echo"))
    {
        if(gargc != 2)
        {
            cout<<"echo:append one"<<endl;
        }
        if(gargc == 2 && gargv[1][0] == '$')
        {
            if(0 == strcmp(gargv[1]+1, "PATH"))
            {
                cout<<getenv("PATH")<<endl;
            }
            else if(0 == strcmp(gargv[1]+1,"?"))
            {
                cout<<lastcode<<endl;
            }
            lastcode = 0;
        }
        return 1; 
    }
    return 0;
}


int main()
{
    //0.加载配置文件 ---暂且先不管 完成核心功能后面再来补充
    //LoadEnv(); 正常使用父进程的 enviroment
    char command_line[MAXSIZE] = {0};
    while(1)
    {
        //1.打印用户名@ 主机名 和文件路径
        PrintCommandLine();

        //2.获取用户输入
        if(GetCommandLine(command_line,sizeof(command_line))==0)
            continue;

        //3.解决重定向问题 解析 追加<< 输入<  输出>
        //"ls -a -l << log.txt"--> "ls -a -l"\0\0 "log.txt"||"ls -a -l < log.txt"-->"ls -a -l"\0"log.txt"|| > 也是一样的道理 目的就是成这样
        ParseRedir(command_line);

        //4.解析命令行 "ls -a -l" ->"ls" "-a" "-l"
        SepCommandLine(command_line);

        //5.检查内建命令
        if(ExcuteBuiltInCommandLine())//0为假，非0为真
            continue;

        //6.执行命令行 看到运行结果
        StartCommandLine();
    }

    


    return 0;
}
