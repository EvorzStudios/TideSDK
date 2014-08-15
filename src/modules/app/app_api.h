/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef _APP_API__H_
#define _APP_API__H_

#ifdef __cplusplus
#include <tide/tide.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#endif

#if defined(OS_OSX) || defined(OS_LINUX)
#define EXPORT __attribute__((visibility("default")))
#define TIDESDK_APP_API EXPORT
#elif defined(OS_WIN32)
# ifdef TIDESDK_APP_API_EXPORT
#  define TIDESDK_APP_API __declspec(dllexport)
# else
#  define TIDESDK_APP_API __declspec(dllimport)
# endif
# define EXPORT __declspec(dllexport)
#endif

#endif
