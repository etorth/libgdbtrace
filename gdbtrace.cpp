/*
 * =====================================================================================
 *
 *       Filename: gdbtrace.cpp
 *        Created: 10/23/2017 09:55:37
 *  Last Modified: 10/23/2017 10:10:26
 *
 *    Description: 
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#include <mutex>
#include <ctime>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "gdbtrace.h"

static void __default_record_handler(const char *szLog)
{
    static const auto szFileName = []() -> std::string
    {
        std::string szFullFileName;

        szFullFileName += std::getenv("HOME");
        szFullFileName += "/.backtrace_record.";

        auto stTime      =  std::time(nullptr);
        auto stLocalTime = *std::localtime(&stTime);

        char szTimeBuf[256];
        std::strftime(szTimeBuf, sizeof(szTimeBuf), "%Y.%m.%d.%H.%M.%S", &stLocalTime);
        szFullFileName += szTimeBuf;

        struct timeval stTimeVal;
        gettimeofday(&stTimeVal, nullptr);

        char szUSecBuf[128];
        std::sprintf(szUSecBuf, ".%07ld.", (long)(stTimeVal.tv_usec));
        szFullFileName += szUSecBuf;

        std::srand(std::time(0));
        for(int nIndex = 0; nIndex < 5; ++nIndex){
            szFullFileName += std::to_string(std::rand() % 10);
        }

        if(auto pAddName = std::getenv("GDBTRACE_RECORD_SUFFIX")){
            szFullFileName += ".";
            szFullFileName += pAddName;
        }

        szFullFileName += ".";
        szFullFileName += std::to_string((unsigned long)(getpid()));

        szFullFileName += ".txt";
        return szFullFileName;
    }();

    static std::mutex s_LogLock;
    {
        std::lock_guard<std::mutex> stLockGuard(s_LogLock);
        if(auto fp = std::fopen(szFileName.c_str(), "a+")){
            std::fprintf(fp, "%s\n", szLog);
            std::fclose(fp);
        }
    }
}

void __gdbtrace_record(__gdbtrace_record_handler fnCB)
{
    if(fnCB == nullptr){
        fnCB = __default_record_handler;
    }

    char szPIDBuf[128];
    std::sprintf(szPIDBuf, "%lu", (unsigned long)(getpid()));

    char szNameBuf[1024];
    szNameBuf[readlink("/proc/self/exe", szNameBuf, sizeof(szNameBuf) - 1)] = '\0';

    if(auto nChildPID = fork()){
        waitpid(nChildPID, nullptr, 0);
    }else{
        dup2(2, 1);
        execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", szNameBuf, szPIDBuf, nullptr);

        // abort here
        // if failed to start gdb
        std::abort();
    }
}
