#include "Logger.hpp"

using namespace LoggerMould;

int main()
{
    ENABLE_FILE_LOG_STRATEGY();

    LOG(LogLevel::DEBUG) << "hell world" << " 3.14 " << 109 << " hello1 ";
    LOG(LogLevel::WARNING) << "hell world" << " 3.14 " << 109 << " hello2 ";
    LOG(LogLevel::FATAL) << "hell world" << " 3.14 " << 109 << " hello3 ";
    LOG(LogLevel::ERROR) << "hell world" << " 3.14 " << 109 << " hello4 ";
    LOG(LogLevel::INFO) << "hell world" << " 3.14 " << 109 << " hello5 ";

    ENABLE_CONSOLE_LOG_STRATEGY();
    LOG(LogLevel::DEBUG) << "hell world" << " 3.14 " << 109 << " hello6 ";
    LOG(LogLevel::WARNING) << "hell world" << " 3.14 " << 109 << " hello7";
    LOG(LogLevel::FATAL) << "hell world" << " 3.14 " << 109 << " hello8 ";
    LOG(LogLevel::ERROR) << "hell world" << " 3.14 " << 109 << " hello9 ";
    LOG(LogLevel::INFO) << "hell world" << " 3.14 " << 109 << " hello10 ";
    // std::cout<<GetCurrentTime()<<std::endl;
    // FilesyStrategy fs;
    // fs.SyncLog("简单测试000");
    // ENABLE_CONSOLE_LOG_STRATEGY();
    // LOG(LogLevel::INFO)<<"sb";
    return 0;
}