#include "utility.h"

char* DateTimeStr(time_t* mytime)
{
    static char s[50];
    s[0] = '\0';
    struct tm* curr;

    curr = localtime(mytime);
    if (NULL == curr)
    {
        return s;
    }

    if (curr->tm_year > 50)
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
            curr->tm_year + 1900, curr->tm_mon + 1, curr->tm_mday,
            curr->tm_hour, curr->tm_min, curr->tm_sec);
    }
    else
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
            curr->tm_year + 2000, curr->tm_mon + 1, curr->tm_mday,
            curr->tm_hour, curr->tm_min, curr->tm_sec);
    }

    return s;
}