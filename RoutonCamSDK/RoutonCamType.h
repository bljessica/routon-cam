#ifndef ROUTONCAMTYPE_H
#define ROUTONCAMTYPE_H

#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <time.h>

#include <HCNetSDK.h>
#include <LinuxPlayM4.h>

#ifdef _WIN32
#elif defined(__linux__) || defined(__APPLE__)
#include   <unistd.h>
#endif

// get data callback function
//typedef void (*gDataCallBack)(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize,void* dwUser);
typedef void (*gDataCallBack)(int width, int height, BYTE* pBuffer);

#endif // ROUTONCAMTYPE_H
