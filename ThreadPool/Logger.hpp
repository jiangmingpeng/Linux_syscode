#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include "Mutex.hpp"

namespace LoggerMould
{
    // 日志等级
    enum class LogLevel // 加class 限定作用域 防止重定义枚举常量
    {
        INFO,
        WARNING,
        ERROR,
        FATAL, // 致命的错误
        DEBUG
    };

    //声明顺序要弄清楚
    std::string LogLever2Messages(LogLevel &level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        default:
            return "UNKNOWN";
        }
    }

    // 获取时间戳 得到当前时间段的年月日时分秒
    std::string GetCurrentTime()
    {

        struct timeval tv;          // 这个结构体成员有 秒和微秒成员
        gettimeofday(&tv, nullptr); // 系统调用，获取秒+微秒 输入输出型参数

        struct tm tm_buf;
        localtime_r(&tv.tv_sec, &tm_buf); // 输入输出型参数

        char buffer[64];
        // strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_buf);//接口函数直接可以取出这些成员变量得到各个时间
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d.%ld" // 这个输出格式要了解
                 ,
                 tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
                 tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec,
                 tv.tv_usec);
        return std::string(buffer);
    }

    class LogStrategy // 采用策略模式来对日志的保存内容进行
    {
    public:
        virtual void SyncLog(const std::string &messages) = 0; // 要被重写的
        virtual ~LogStrategy()
        {
        }
    };

    // 控制台策略
    class ConsoleStrategy : public LogStrategy
    {
    public:
        void SyncLog(const std::string &messages) override
        {
            MutexGuard lock(_mutex);            // 对控制台输出做互斥保护 防止并发导致数据不一致问题
            std::cout << messages << std::endl; // 如果不考虑互斥问题 其实就这一条语句
        }
        ~ConsoleStrategy()
        {
        }

    private:
        MutexMould _mutex;
    };

    // 入文件策略
    class FilesyStrategy : public LogStrategy
    {
    public:
        FilesyStrategy(const std::string &filepath = "./log", const std::string &filename = "log.txt")
            : _logfilepath(filepath), _logfilename(filename)
        {
            MutexGuard lock(_mutex);
            if (std::filesystem::exists(_logfilepath))
                return;
        }
        void SyncLog(const std::string &messages) override
        {
            {
                MutexGuard lock(_mutex);
                if (!_logfilepath.empty() && _logfilepath.back() != '/')
                {
                    _logfilepath += "/";
                }
                // 下面的写法需要考经验堆出来
                std::string targetlog = _logfilepath + _logfilename;       // "./log/log.txt"
                namespace fs = std::filesystem;                            // 命名空间重命名
                fs::create_directories(fs::path(targetlog).parent_path()); // 目录不存在就创建目录
                std::ofstream out(targetlog, std::ios::app);               // 对一个文件进行追加方式写入 后面是操作的标志位
                if (!out.is_open())
                {
                    std::cerr << "open " << targetlog << " failed" << std::endl;
                    return;
                }
                out << messages << "\n"; // 把内容写入该打开的文件 这个文件流了解就可以 下次看见不觉得奇怪即可
                out.close();             // 每次写入都进行打开关闭的操作
            }
        }
        ~FilesyStrategy()
        {
        }

    private:               // 这个日志打印的文件 感觉可以有所改进
        MutexMould _mutex; // 这个锁的对象生命周期
        std::string _logfilepath;
        std::string _logfilename; // 目的是在log目录下生成一个log.txt的文件用来写入内容 所以路径的表示要绝对正确
    };
    // 以上是两种输出策略

    // 下面是对外的日志接口
    class Logger
    {
    public:
        Logger()
        {
            UseConsoleStrategy(); // 默认使用控制台策略
        }

        void UseConsoleStrategy()
        {
            _strategy = std::make_unique<ConsoleStrategy>();
        }

        void UseFilesyStrategy()
        {
            _strategy = std::make_unique<FilesyStrategy>();
        }

        ~Logger()
        {
        }

        class LoggerMessages
        {
        public:
            LoggerMessages(LogLevel level, std::string filename, int line, Logger &Ref)
                : _curr_time(GetCurrentTime()), _level(level), _pid(getpid()), _filename(filename), _line(line), _logger(Ref)
            {
                std::stringstream ss; // 作用类似snprintf 把内容写入字符串
                ss << "[" << _curr_time << "] "
                   << "[" << LogLever2Messages(_level) << "] "
                   << "[" << _pid << "] "
                   << "[" << _filename << "] "
                   << "[" << _line << "]"
                   << " - ";
                _info = ss.str();
            }
            template <typename T>
            LoggerMessages &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;        // 因为这个<<已经向cout对齐 后面直接转即可
                _info += ss.str(); // 直接加等再进行返回对象 再次调用重载符 注意这个加等的类型要转化成一样 因为是T类型的
                return *this;      // 返回当前LoggerMessages对象，方便下次继续进行<<
            }
            ~LoggerMessages()
            {
                if (_logger._strategy) // 只要策略指针存在就会调用析构
                {
                    _logger._strategy->SyncLog(_info);
                }
            }

        private:
            std::string _curr_time;
            LogLevel _level;
            pid_t _pid;
            std::string _filename;
            int _line;

            std::string _info; // 一条完整的日志信息
            Logger &_logger;
        };

        // 在用外部类的重载的()时就会触发内部的构造和析构
        LoggerMessages operator()(LogLevel level, std::string filename, int line)
        {
            return LoggerMessages(level, filename, line, *this); // 第四个参数传的是基类本身
        }

    private:
        std::unique_ptr<LogStrategy> _strategy; // 派生类的指针和引用可以赋给基类 可能导致切片
    };

    Logger logger;



#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleStrategy();
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFilesyStrategy();
//上面的全局函数调用后即会 让logger进入到最后一个调用的模式 进而后续影响LOG的功能
#define LOG(level) logger(level,__FILE__,__LINE__)


}
