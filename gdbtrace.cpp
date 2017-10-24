/*
 * =====================================================================================
 *
 *       Filename: gdbtrace.cpp
 *        Created: 10/23/2017 09:55:37
 *  Last Modified: 10/24/2017 11:44:52
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

extern "C" void __gdbtrace_record()
{
    char szPIDBuf[128];
    std::sprintf(szPIDBuf, "%lu", (unsigned long)(getpid()));

    char szNameBuf[1024];
    szNameBuf[readlink("/proc/self/exe", szNameBuf, sizeof(szNameBuf) - 1)] = '\0';

    switch(auto nPID = fork()){
        case -1:
            {
                return;
            }
        case  0:
            {
                waitpid(nPID, nullptr, 0);
                return;
            }
        default:
            {
                signal(SIGABRT, SIG_DFL);
                execl("/home/anhong/libgdbtrace/gdbtrace", "gdbtrace", szNameBuf, szPIDBuf, "--log-file", "--log-append", nullptr);

                std::abort();
                return;
            }
    }
}
