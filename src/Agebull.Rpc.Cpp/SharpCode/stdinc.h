#ifndef ___STDINC_H
#define ___STDINC_H
#pragma once
//�ⲿ���ͳһ����
#ifdef CLR
#pragma unmanaged
#endif

#ifdef CLIENT
#define CLIENT_COMMAND
#endif

#ifdef WEB
#define CLIENT_COMMAND
#endif
#include "mydecimal.h"

#include "zeromq/zmq.h"

#include <cassert>
#include <stdio.h>
#include <tchar.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <memory>
#include <queue>

#ifdef WIN32
typedef unsigned long ulong;
typedef unsigned int uint;
typedef __int64 int64_t;
typedef __int64 int64;
typedef unsigned __int64 uint64;
#endif

typedef unsigned short ushort;
typedef unsigned short u_short;
typedef unsigned char uchar;
typedef unsigned char u_char;
typedef unsigned int u_int;

//boost��CLR������,�����ڷ����
#ifdef CLR
#pragma managed
#include <msclr\marshal.h>
#pragma unmanaged

#endif
#ifdef NOCLR
#include "acl.h"
#include "boostinc.h"
//�߳��ж�
#define thread_sleep(n) boost::this_thread::sleep(boost::posix_time::milliseconds(n));
//�����߳�
#define start_thread(func, args) \
{\
	boost::thread thread_xxx(boost::bind<DWORD>(&func, args));\
}
#else
//�����߳�
inline HANDLE start_thread(LPTHREAD_START_ROUTINE func, LPVOID args)
{
	DWORD threadID;
	return CreateThread(nullptr, 0, func, args, 0, &threadID); // �����߳�
}
//�߳��ж�
#define thread_sleep(n) Sleep(n);
#endif

#include "log/mylogger.h"

#endif

#include "tm_extend.h"
#include "common.h"
using namespace std;