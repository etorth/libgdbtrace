/*
 * =====================================================================================
 *
 *       Filename: gdbtrace.h
 *        Created: 10/23/2017 09:53:01
 *  Last Modified: 10/23/2017 10:12:49
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

#ifndef _GDBTRACE_0682549163_H_
#define _GDBTRACE_0682549163_H_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

typedef void (*__gdbtrace_record_handler)(const char *);
EXTERNC void __gdbtrace_record(__gdbtrace_record_handler);
#endif
