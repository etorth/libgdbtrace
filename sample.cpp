/*
 * =====================================================================================
 *
 *       Filename: sample.cpp
 *        Created: 10/23/2017 10:10:43
 *  Last Modified: 10/23/2017 10:11:27
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

#include "gdbtrace.h"
int f(int)
{
    __gdbtrace_record(nullptr);
    return 0;
}

int main()
{
    return f(2);
}
