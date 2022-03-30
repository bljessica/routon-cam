#include <RoutonCamCommon.h>
#include <RoutonCamType.h>

void Routon_GetSysTime(char *time)
{
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv, nullptr);
    tm = localtime(&tv.tv_sec);
    sprintf(time,"[%d-%02d-%02d %02d:%02d:%02d]",tm->tm_year + 1900,tm->tm_mon + 1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
}
