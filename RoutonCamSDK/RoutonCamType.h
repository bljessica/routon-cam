#pragma once
//#ifndef ROUTONCAMTYPE_H
//#define ROUTONCAMTYPE_H

#include <stdio.h>
#include <string.h>

//#include <sys/time.h>
#include <time.h>

//#include <DataType.h>
#include <HCNetSDK.h>
#include <plaympeg4.h>
//#include <LinuxPlayM4.h>


// get data callback function
//typedef void (*gDataCallBack)(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize,void* dwUser);
typedef void (*gDataCallBack)(int width, int height, BYTE* pBuffer);

