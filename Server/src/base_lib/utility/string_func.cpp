#include "string_func.h"
#include <assert.h>

// copy string
void StrCpy(char* szDest, const char* szSrc, int iBufLen)
{
    if (NULL == szDest || NULL == szSrc)
    {
        assert(false);
        return;
    }

    strncpy(szDest, szSrc, iBufLen - 1);
    szDest[iBufLen - 1] = 0;
}

int is_leap_year(int year)
{
    return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}

int match_string(const char** buf, const char** strs)
{
    int i = 0;

    for (i = 0; strs[i] != NULL; ++i) {
        size_t len = strlen(strs[i]);

        if (strnicmp(*buf, strs[i], len) == 0) {
            *buf += len;
            return i;
        }
    }
    return -1;
}

char* tdr_strptime(const char* buf, const char* format, struct tm* timeptr)
{
    char c;

    for (; (c = *format) != '\0'; ++format) {
        char* s;
        int ret;

        if (isspace(c)) {
            while (isspace(*buf))
                ++buf;
        }
        else if (c == '%' && format[1] != '\0') {
            c = *++format;
            if (c == 'E' || c == 'O')
                c = *++format;
            switch (c) {
            case 'A':
                ret = match_string(&buf, full_weekdays);
                if (ret < 0)
                    return NULL;
                timeptr->tm_wday = ret;
                break;
            case 'a':
                ret = match_string(&buf, abb_weekdays);
                if (ret < 0)
                    return NULL;
                timeptr->tm_wday = ret;
                break;
            case 'B':
                ret = match_string(&buf, full_month);
                if (ret < 0)
                    return NULL;
                timeptr->tm_mon = ret;
                break;
            case 'b':
            case 'h':
                ret = match_string(&buf, abb_month);
                if (ret < 0)
                    return NULL;
                timeptr->tm_mon = ret;
                break;
            case 'C':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_year = (ret * 100) - tm_year_base;
                buf = s;
                break;
            case 'c':        /* %a %b %e %H:%M:%S %Y */
                s = tdr_strptime(buf, "%a %b %e %H:%M:%S %Y", timeptr);
                if (s == NULL)
                    return NULL;
                buf = s;
                break;
            case 'D':        /* %m/%d/%y */
                s = tdr_strptime(buf, "%m/%d/%y", timeptr);
                if (s == NULL)
                    return NULL;
                buf = s;
                break;
            case 'd':
            case 'e':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_mday = ret;
                buf = s;
                break;
            case 'H':
            case 'k':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_hour = ret;
                buf = s;
                break;
            case 'I':
            case 'l':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                if (ret == 12)
                    timeptr->tm_hour = 0;
                else
                    timeptr->tm_hour = ret;
                buf = s;
                break;
            case 'j':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_yday = ret - 1;
                buf = s;
                break;
            case 'm':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_mon = ret - 1;
                buf = s;
                break;
            case 'M':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_min = ret;
                buf = s;
                break;
            case 'n':
                if (*buf == '\n')
                    ++buf;
                else
                    return NULL;
                break;
            case 'p':
                ret = match_string(&buf, ampm);
                if (ret < 0)
                    return NULL;
                if (timeptr->tm_hour == 0) {
                    if (ret == 1)
                        timeptr->tm_hour = 12;
                }
                else
                    timeptr->tm_hour += 12;
                break;
            case 'r':        /* %I:%M:%S %p */
                s = tdr_strptime(buf, "%I:%M:%S %p", timeptr);
                if (s == NULL)
                    return NULL;
                buf = s;
                break;
            case 'R':        /* %H:%M */
                s = tdr_strptime(buf, "%H:%M", timeptr);
                if (s == NULL)
                    return NULL;
                buf = s;
                break;
            case 'S':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_sec = ret;
                buf = s;
                break;
            case 't':
                if (*buf == '\t')
                    ++buf;
                else
                    return NULL;
                break;
            case 'T':        /* %H:%M:%S */
            case 'X':
                s = tdr_strptime(buf, "%H:%M:%S", timeptr);
                if (s == NULL)
                    return NULL;
                buf = s;
                break;
            case 'u':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_wday = ret - 1;
                buf = s;
                break;
            case 'w':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_wday = ret;
                buf = s;
                break;
            case 'U':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                set_week_number_sun(timeptr, ret);
                buf = s;
                break;
            case 'V':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                set_week_number_mon4(timeptr, ret);
                buf = s;
                break;
            case 'W':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                set_week_number_mon(timeptr, ret);
                buf = s;
                break;
            case 'x':
                s = tdr_strptime(buf, "%Y:%m:%d", timeptr);
                if (s == NULL)
                    return NULL;
                buf = s;
                break;
            case 'y':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                if (ret < 70)
                    timeptr->tm_year = 100 + ret;
                else
                    timeptr->tm_year = ret;
                buf = s;
                break;
            case 'Y':
                ret = strtol(buf, &s, 10);
                if (s == buf)
                    return NULL;
                timeptr->tm_year = ret - tm_year_base;
                buf = s;
                break;
            case 'Z':
                /* Unsupported. Just ignore.  */
                break;
            case '\0':
                --format;
                /* FALLTHROUGH */
            case '%':
                if (*buf == '%')
                    ++buf;
                else
                    return NULL;
                break;
            default:
                if (*buf == '%' || *++buf == c)
                    ++buf;
                else
                    return NULL;
                break;
            }
        }
        else {
            if (*buf == c)
                ++buf;
            else
                return NULL;
        }
    }
    return (char*)buf;
}

void set_week_number_mon(struct tm* timeptr, int wnum)
{
    int fday = (first_day(timeptr->tm_year + tm_year_base) + 6) % 7;

    timeptr->tm_yday = wnum * 7 + (timeptr->tm_wday + 6) % 7 - fday;
    if (timeptr->tm_yday < 0) {
        timeptr->tm_wday = (fday + 1) % 7;
        timeptr->tm_yday = 0;
    }
}

/*
* Set `timeptr' given `wnum' (week number [0, 53])
* Needed for tdr_strptime
*/
void set_week_number_mon4(struct tm* timeptr, int wnum)
{
    int fday = (first_day(timeptr->tm_year + tm_year_base) + 6) % 7;
    int offset = 0;

    if (fday < 4)
        offset += 7;

    timeptr->tm_yday = offset + (wnum - 1) * 7 + timeptr->tm_wday - fday;
    if (timeptr->tm_yday < 0) {
        timeptr->tm_wday = fday;
        timeptr->tm_yday = 0;
    }
}

void set_week_number_sun(struct tm* timeptr, int wnum)
{
    int fday = first_day(timeptr->tm_year + tm_year_base);

    timeptr->tm_yday = wnum * 7 + timeptr->tm_wday - fday;
    if (timeptr->tm_yday < 0) {
        timeptr->tm_wday = fday;
        timeptr->tm_yday = 0;
    }
}
int first_day(int year)
{
    int ret = 4;

    while (--year >= 1970)
        ret = (ret + 365 + is_leap_year(year)) % 7;
    return ret;
}