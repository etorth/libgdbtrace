/*
 * =====================================================================================
 *
 *       Filename: main.cpp
 *        Created: 10/23/2017 17:57:06
 *  Last Modified: 10/24/2017 11:35:24
 *
 *    Description: usage as
 *                     gdbtrace binfile pid
 *                     gdbtrace binfile pid
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

#include <ctime>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

static bool __option_match(const char *szInput, const char *szOpt)
{
    if(szInput && szOpt){
        int nIndex = 0;
        for(; szInput[nIndex] && szOpt[nIndex]; ++nIndex){
            if(szInput[nIndex] != szOpt[nIndex]){
                return false;
            }
        }

        // szInput = --prefixabc
        // szOpt   = --prefix

        return szOpt[nIndex] == '\0' && (szInput[nIndex] == '=' || szInput[nIndex] == '\0');
    }
    return false;
}

static int __option_value(const char *szInput, const char *szOpt)
{
    if(__option_match(szInput, szOpt)){
        auto nOptLen = std::strlen(szOpt);
        switch(szInput[nOptLen]){
            case '\0':
                {
                    return nOptLen;
                }
            case '=':
                {
                    return nOptLen + 1;
                }
            default:
                {
                    return -1;
                }
        }
    }
    return -1;
}

static FILE *__do_gdb_command_list(const char *szBinName, const char *szParentPID)
{
    char szGDBCommand[1024];
    std::sprintf(szGDBCommand, "gdb --batch -n -ex thread -ex bt %s %s", szBinName, szParentPID);
    return popen(szGDBCommand, "r");
}

static const char *__default_gdb_output_filter(const char *szOutputLine)
{
    if(true
            && szOutputLine
            && szOutputLine[0] == '#'
            && szOutputLine[1] >= '0'
            && szOutputLine[1] <= '9'){
        return szOutputLine;
    }
    return nullptr;
}

int main(int argc, char *argv[])
{
    sleep(1);
    sleep(1);

    std::vector<std::string> szvArg;
    for(int nIndex = 1; nIndex < argc; ++nIndex){
        szvArg.emplace_back(argv[nIndex]);
    }

    bool bFileOutput = false;
    std::string szOutputFile;

    // find --log-file=xxx
    {
        for(size_t nIndex = 0; nIndex < szvArg.size(); ++nIndex){
            auto nLocVal = __option_value(szvArg[nIndex].c_str(), "--log-file");
            if(nLocVal >= 0){
                bFileOutput = true;
                if(szvArg[nIndex][nLocVal] == '\0'){
                    // matches, but no value provided
                }else{
                    szOutputFile = szvArg[nIndex].c_str() + nLocVal;
                }

                // there is size check
                // so after calling erase break immediately
                szvArg.erase(szvArg.begin() + nIndex);
                break;
            }else{
                // not even match
            }
        }

        if(bFileOutput && szOutputFile.empty()){

            szOutputFile += std::getenv("HOME");
            szOutputFile += "/.gdbtrace.";

            auto stTime      =  std::time(nullptr);
            auto stLocalTime = *std::localtime(&stTime);

            char szTimeBuf[256];
            std::strftime(szTimeBuf, sizeof(szTimeBuf), "%Y.%m.%d.%H.%M.%S", &stLocalTime);
            szOutputFile += szTimeBuf;

            struct timeval stTimeVal;
            gettimeofday(&stTimeVal, nullptr);

            char szUSecBuf[128];
            std::sprintf(szUSecBuf, ".%07ld.", (long)(stTimeVal.tv_usec));
            szOutputFile += szUSecBuf;

            std::srand(std::time(0));
            for(int nIndex = 0; nIndex < 5; ++nIndex){
                szOutputFile += std::to_string(std::rand() % 10);
            }

            if(auto pAddName = std::getenv("GDBTRACE_SUFFIX")){
                szOutputFile += ".";
                szOutputFile += pAddName;
            }

            szOutputFile += ".";
            szOutputFile += std::to_string((unsigned long)(getpid()));

            szOutputFile += ".txt";
        }
    }

    bool bClearOutputFile = true;
    {
        for(size_t nIndex = 0; nIndex < szvArg.size(); ++nIndex){
            auto nLocVal = __option_value(szvArg[nIndex].c_str(), "--log-append");
            if(nLocVal >= 0){
                bClearOutputFile = false;
                // there is size check
                // so after calling erase break immediately
                szvArg.erase(szvArg.begin() + nIndex);
                break;
            }
        }
    }

    // at least we have ``binfile" and ``pid"
    if(szvArg.size() < 2){
        return 1;
    }

    std::string szContent;
    if(auto fp = __do_gdb_command_list(szvArg[0].c_str(), szvArg[1].c_str())){
        char szOutputLine[1024];
        while(std::fgets(szOutputLine, sizeof(szOutputLine), fp)){
            // remove tailing newline
            // then feed to the log handler
            szOutputLine[std::strcspn(szOutputLine, "\r\n")] = '\0';
            if(auto szFilterOut = __default_gdb_output_filter(szOutputLine)){
                szContent += szFilterOut;
                szContent += "\n";
            }
        }
        pclose(fp);
    }

    if(!szContent.empty()){
        if(!bFileOutput){
            std::fprintf(stdout, "%s", szContent.c_str());
            return 0;
        }else{
            if(auto fp = std::fopen(szOutputFile.c_str(), bClearOutputFile ? "w+" : "a+")){
                std::fprintf(fp, "%s", szContent.c_str());
                std::fclose(fp);
                return 0;
            }
            return 1;
        }
    }
    return 0;
}
