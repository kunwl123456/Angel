#ifndef STRING_FUNC_H_
#define STRING_FUNC_H_

#include <string>

#if defined(_WIN32) || defined(_WIN64)
#define  tdr_pal_strptime tdr_strptime
#else
#define tdr_pal_strptime    strptime
#endif//

static const char* abb_weekdays[] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    NULL
};

static const char* full_weekdays[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    NULL
};

static const char* abb_month[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
    NULL
};

static const char* full_month[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
    NULL,
};
static const char* ampm[] = {
    "am",
    "pm",
    NULL
};
static const int tm_year_base = 1900;
void StrCpy(char* szDest, const char* szSrc, int iBufLen);
static int is_leap_year(int year);
static int match_string(const char** buf, const char** strs);
char* tdr_strptime(const char* buf, const char* format, struct tm* timeptr);
static void set_week_number_mon(struct tm* timeptr, int wnum);
static void set_week_number_mon4(struct tm* timeptr, int wnum);
static void set_week_number_sun(struct tm* timeptr, int wnum);
static int first_day(int year);

#endif //STRING_FUNC_H_
