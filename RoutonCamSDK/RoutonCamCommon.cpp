#include <RoutonCamCommon.h>
#include <RoutonCamType.h>

void Routon_GetSysTime(char* time_str)
{
    time_t now = time(0);
    struct tm* tm = localtime(&now);
    sprintf(time_str, "[%d-%02d-%02d %02d:%02d:%02d]", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}
