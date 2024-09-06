#include "zonetime.h"

int gettimeofday(struct timeval* tp, void* tzp)
#if defined(_WIN32) || defined(_WIN64)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return 0;
}
#else
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tp->tv_sec = ts.tv_sec;
    tp->tv_usec = ts.tv_nsec / 1000;
    return 0;
}
#endif //gettimeofday


CZoneTime::CZoneTime() : m_illLastSecond(0), m_iMaxID(1), m_iGMOffsetSec(0), m_iCfgUpdateTimeAlarmThreshold(3), m_illTimeStrSec(0)
{
    m_iTimeZoneSec = _GetTimeZoneMinute() * ONE_MIN_SECONDS;
    UpdateTime();
}

CZoneTime::~CZoneTime()
{

}

void CZoneTime::UpdateTime(BOOL bCheckModifyTime)
{
    // 获取最新的时间
    gettimeofday(&m_stRealTv, NULL);

    // 判断时间是否跳了
    if (bCheckModifyTime)
    {
        _CheckSysTimeSkip();
    }

    // 重置每秒的ID计数器
    if (m_illLastSecond != m_stRealTv.tv_sec)
    {
        m_illLastSecond = m_stRealTv.tv_sec;
        m_iMaxID = 1;
    }

    // 更新最新的时间戳
    m_stCurrTv.tv_sec = m_stRealTv.tv_sec;
    m_stCurrTv.tv_usec = m_stRealTv.tv_usec;

    // 要带上时间偏移
    m_stCurrTv.tv_sec += m_iGMOffsetSec;
}

// 判断时间变更间隔
void CZoneTime::_CheckSysTimeSkip()
{
    // 如果还没有设置上一次的秒数
    if (0 == m_illLastSecond)
    {
        return;
    }

    // 计算时间偏移的秒数
    int32_t iPassSec = m_stRealTv.tv_sec - m_illLastSecond;
    if (iPassSec > m_iCfgUpdateTimeAlarmThreshold || iPassSec < 0)
    {
        //LOGFATAL_LOCAL(E_ATTR_ID_ALARM_SYS_TIME_SKIP, "check sys time skip, may time skip %d cur:%ld.%ld new:%ld.%ld last_sec:%ld gm_offset:%d",
        //  iPassSec, m_stCurrTv.tv_sec, m_stCurrTv.tv_usec, m_stRealTv.tv_sec, m_stRealTv.tv_usec, m_illLastSecond, m_iGMOffsetSec);
    }
}

const TIME_VAL& CZoneTime::GetCurTime(BOOL bRealTime)
{
    if (bRealTime)
    {
        return m_stRealTv;
    }

    return m_stCurrTv;
}

int64_t CZoneTime::GetCurSecond(BOOL bRealTime)
{
    return  GetCurTime(bRealTime).tv_sec;
}

int32_t CZoneTime::GetOffsetSecOf2020(BOOL bRealTime)
{
    return  static_cast<int32_t>(GetCurTime(bRealTime).tv_sec - COM_SECTIMESTAMP_AT_2020);
}

int64_t CZoneTime::GetSecByOffsetSecOf2020(int32_t iOffsetSec)
{
    return COM_SECTIMESTAMP_AT_2020 + iOffsetSec;
}

int64_t  CZoneTime::GetCurLocalSecond(BOOL bRealTime)
{
    return ConvSecUTCToLocal(GetCurSecond(bRealTime));
}

int32_t CZoneTime::GetCurUSecond(BOOL bRealTime)
{
    return GetCurTime(bRealTime).tv_usec;
}

int32_t CZoneTime::GetCurMSecond(BOOL bRealTime)
{
    return (GetCurTime(bRealTime).tv_usec / 1000);
}

int64_t CZoneTime::GetCurMS(BOOL bRealTime)
{
    return (GetCurTime(bRealTime).tv_sec * 1000 + GetCurTime(bRealTime).tv_usec / 1000);
}

int32_t CZoneTime::GetCurIDInSecond()
{
    static int64_t llLastSec = GetCurSecond();
    if (llLastSec != GetCurSecond())
    {
        llLastSec = GetCurSecond();
        m_iMaxID = 1;
    }

    m_iMaxID++;
    if (m_iMaxID <= 0)
    {
        m_iMaxID = 1;
    }

    return  m_iMaxID;
}

uint64_t CZoneTime::GetCpuTickCnt()
{
    uint32_t a, d;
    uint32_t eax, ebx, ecx, edx;

    asm volatile("cpuid":"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
    asm volatile("rdtsc" : "=a" (a), "=d" (d));

    return (((uint64_t)a) | (((uint64_t)d) << 32));
}

void CZoneTime::CutSecToMidNightSpare(int64_t illNowSec, int64_t& rillMidNightSec, int64_t& rillSpareSec) const
{
    // 首先把当前秒取整到具体的多少天数
    int64_t illIntegerDayNum = illNowSec / ONE_DAY_SECONDS;

    rillMidNightSec = illIntegerDayNum * ONE_DAY_SECONDS;
    rillSpareSec = illNowSec - rillMidNightSec;
}

int64_t CZoneTime::ConvSecUTCToLocal(int64_t illSec) const
{
    return illSec - m_iTimeZoneSec;
}

int64_t CZoneTime::ConvSecLocalToUTC(int64_t illSec) const
{
    return illSec + m_iTimeZoneSec;
}

/*!!
iDays + 3是以周一作为一周开始分割
iDays + 4是以周日作为一周开始分割
*/
int32_t CZoneTime::GetWeekNo(int64_t illTime)
{
    int64_t illLocalTime = ConvSecUTCToLocal(illTime);

    // 1970-1-1是周4
    int32_t iDays = illLocalTime / ONE_DAY_SECONDS;
    return ((iDays + 3) / 7);
}

int32_t CZoneTime::GetTimeZoneSecOppositeNum(BOOL bRealTime /*= false*/)
{
    if (bRealTime)
    {
        return -_GetTimeZoneMinute() * ONE_MIN_SECONDS;
    }

    return -GetTimeZoneSec();
}

int32_t CZoneTime::GetHourInDay(int64_t illTime)
{
    int64_t illLocalNowTime = ConvSecUTCToLocal(illTime);

    int64_t illMidNightSec, illSpareSec;
    CutSecToMidNightSpare(illLocalNowTime, illMidNightSec, illSpareSec);

    return  ((int32_t)(illSpareSec / ONE_HOUR_SECONDS)) % 24;
    /*
    time_t tTime = dwTime;
    struct tm Tm;

    if (!localtime_r(&tTime,&Tm))
        return    -1;

    return    Tm.tm_hour;
    */
}

int32_t CZoneTime::GetMinInHour(int64_t illTime)
{
    int64_t illLocalTime = ConvSecUTCToLocal(illTime);

    int64_t illMidNightSec, illSpareSec;
    CutSecToMidNightSpare(illLocalTime, illMidNightSec, illSpareSec);

    return  ((int32_t)(illSpareSec / ONE_MIN_SECONDS)) % 60;
    /*
    time_t tTime = dwTime;
    struct tm Tm;

    if (!localtime_r(&tTime,&Tm))
        return    -1;

    return    Tm.tm_min;
    */
}

int32_t CZoneTime::GetSecInDay(int64_t illTime)
{
    int64_t illLocalTime = ConvSecUTCToLocal(illTime);

    int64_t illMidNightSec, illSpareSec;
    CutSecToMidNightSpare(illLocalTime, illMidNightSec, illSpareSec);

    return  (int32_t)illSpareSec;
}

int64_t CZoneTime::GetZeroTimeOfDay(int64_t llTime)
{
    return llTime - GetSecInDay(llTime);
}

int32_t CZoneTime::GetDayInMonthSplitTime(int32_t iSplitTime)
{
    time_t tCurTime = GetCurSecond();

    time_t tCurSplitTime = ChgSplitTime2Sys(iSplitTime, tCurTime);
    time_t tMinSplitTime = 0, tMaxSplitTime = 0;

    if (tCurSplitTime <= tCurTime)
    {
        tMinSplitTime = tCurSplitTime;
    }
    else
    {
        tMinSplitTime = tCurSplitTime - ONE_DAY_SECONDS;
    }

    const ST_DATE& rstDate = ConvertSecToDate(tMinSplitTime);
    return rstDate.m_iDay;
}

int32_t CZoneTime::GetMonthSplitTime(int32_t iSplitTime)
{
    time_t tCurTime = GetCurSecond();

    time_t tCurSplitTime = ChgSplitTime2Sys(iSplitTime, tCurTime);
    time_t tMinSplitTime = 0, tMaxSplitTime = 0;

    if (tCurSplitTime <= tCurTime)
    {
        tMinSplitTime = tCurSplitTime;
    }
    else
    {
        tMinSplitTime = tCurSplitTime - ONE_DAY_SECONDS;
    }

    const ST_DATE& rstDate = ConvertSecToDate(tMinSplitTime);
    return rstDate.m_iMonth;
}


int32_t CZoneTime::GetTotalDaysInCurrentMonth(int64_t illSec)
{
    //获取月份
    const ST_DATE& rstDate = ConvertSecToDate(illSec);

    int iDaysInMonth[ONE_YEAR_MONTHS] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    int32_t iTotalDaysInCurrentMonth = iDaysInMonth[(rstDate.m_iMonth - 1) % ONE_YEAR_MONTHS];

    //判断闰年
    if (((0 == rstDate.m_iYear % 4) && (0 != rstDate.m_iYear % 100)) || (0 == rstDate.m_iYear % 400))
    {
        if (2 == rstDate.m_iMonth)
        {
            iTotalDaysInCurrentMonth++;
        }
    }

    return iTotalDaysInCurrentMonth;
}

int32_t CZoneTime::CalAlternateDays(int64_t illNow, int64_t illLast)
{
    if (0 == illLast || illNow <= illLast)
    {
        return 0;
    }

    int64_t illLocalNow = ConvSecUTCToLocal(illNow);

    int64_t illNowMidNightSec, illNowSpareSec;
    CutSecToMidNightSpare(illLocalNow, illNowMidNightSec, illNowSpareSec);

    int64_t illLocalLast = ConvSecUTCToLocal(illLast);

    int64_t illLastMidNightSec, illLastSpareSec;
    CutSecToMidNightSpare(illLocalLast, illLastMidNightSec, illLastSpareSec);

    int64_t illMidNightSecDiff = illNowMidNightSec - illLastMidNightSec;
    return   (int32_t)(illMidNightSecDiff / ONE_DAY_SECONDS);

    /*
    struct tm *pstNowTime = localtime( &lNow );
    struct tm tTime = {0};
    tTime.tm_year  = pstNowTime->tm_year;
    tTime.tm_mon   = pstNowTime->tm_mon;
    tTime.tm_mday  = pstNowTime->tm_mday;
    lNow = mktime( &tTime );

    struct tm *pstLastTime = localtime( &lLast );
    struct tm tLastTime = {0};
    tLastTime.tm_year = pstLastTime->tm_year;
    tLastTime.tm_mon  = pstLastTime->tm_mon;
    tLastTime.tm_mday = pstLastTime->tm_mday;
    lLast = mktime( &tLastTime );
    long lTime = lNow - lLast;
    lTime = lTime / 86400;// 计算天数

    return lTime;
    */
}

int64_t CZoneTime::GetMondaySplitTime(int32_t iSplitTime)
{
    time_t    tCurTime = GetCurSecond();
    return GetMondaySplitTime(iSplitTime, tCurTime);
}

int64_t CZoneTime::GetMondaySplitTime(int32_t iSplitTime, int64_t llNow) const
{
    int          iWDay = GetWDay(llNow);
    int64_t illSplitToday = ChgSplitTime2Sys(iSplitTime, llNow);

    if (0 == iWDay)
    {
        return illSplitToday - 6 * ONE_DAY_SECONDS;
    }
    else
    {
        return illSplitToday - (iWDay - 1) * ONE_DAY_SECONDS;
    }
}

int64_t CZoneTime::GetLastMondaySplitTime(int32_t iSplitTime)
{
    int64_t illMondaySplitTime = GetMondaySplitTime(iSplitTime);
    if (GetCurSecond() < illMondaySplitTime)
    {
        return illMondaySplitTime - ONE_WEEK_SECONDS;
    }
    else
    {
        return illMondaySplitTime;
    }
}

//比如iSplitTime是0500，表示每天5:00刷新，返回tTime1和tTime2是否根据在同一天内(同一天是指从5：00到次日05:00)
BOOL CZoneTime::ChkGameInSameDay(int32_t iSplitTime, time_t tTime1, time_t tTime2)
{
    time_t tSplitTime = ChgSplitTime2Sys(iSplitTime, tTime1);
    time_t tMinSplitTime = 0, tMaxSplitTime = 0;

    if (tSplitTime <= tTime1)
    {
        tMinSplitTime = tSplitTime;
        tMaxSplitTime = tSplitTime + ONE_DAY_SECONDS;
    }
    else
    {
        tMinSplitTime = tSplitTime - ONE_DAY_SECONDS;
        tMaxSplitTime = tSplitTime;
    }

    return (tTime2 >= tMinSplitTime && tTime2 < tMaxSplitTime);
}

//比如iSplitTime是0500，表示每天5:00刷新，返回tChkTime和当前时间是否根据在同一天内(同一天是指从5：00到次日05:00)
BOOL CZoneTime::ChkGameInSameDay(int32_t iSplitTime, time_t tChkTime)
{
    if (tChkTime == 0)
    {
        return false;
    }

    time_t tCurTime = GetCurSecond();

    time_t tCurSplitTime = ChgSplitTime2Sys(iSplitTime, tCurTime);
    time_t tMinSplitTime = 0, tMaxSplitTime = 0;

    if (tCurSplitTime <= tCurTime)
    {
        tMinSplitTime = tCurSplitTime;
        tMaxSplitTime = tCurSplitTime + ONE_DAY_SECONDS;
    }
    else
    {
        tMinSplitTime = tCurSplitTime - ONE_DAY_SECONDS;
        tMaxSplitTime = tCurSplitTime;
    }

    return (tChkTime >= tMinSplitTime && tChkTime < tMaxSplitTime);
}

BOOL CZoneTime::ChkGameInSameMonth(int32_t iSplitTime, time_t tChkTime)
{
    if (tChkTime == 0)
    {
        return false;
    }

    time_t tCurTime = GetCurSecond();

    time_t tCurSplitTime = ChgSplitTime2Sys(iSplitTime, tCurTime);
    time_t tCheckSplitTime = ChgSplitTime2Sys(iSplitTime, tChkTime);

    const ST_DATE& rstDate = ConvertSecToDate(tCurSplitTime);
    int32_t iCurMonth = rstDate.m_iMonth;
    int32_t iCurYear = rstDate.m_iMonth;

    const ST_DATE& rstCheckDate = ConvertSecToDate(tCheckSplitTime);
    int32_t iCheckMonth = rstCheckDate.m_iMonth;
    int32_t iCheckYear = rstCheckDate.m_iMonth;

    return (iCurYear == iCheckYear) && (iCheckMonth == iCurMonth);
}

BOOL CZoneTime::ChkGameInSameHour(int32_t iSplitMin, int32_t iSplitSec, time_t tChkTime)
{
    if (0 == tChkTime || iSplitMin < 0 || iSplitMin >= 60 || iSplitSec < 0 || iSplitSec >= 60)
    {
        //LOGERR(ZERO_UID, MODULE_TIME, "invalid tChkTime(%ld) or iSplitMin(%d) or iSplitSec(%d)", tChkTime, iSplitMin, iSplitSec);
        return false;
    }

    int32_t iSplitTotalSec = iSplitMin * ONE_MIN_SECONDS + iSplitSec;

    time_t tAdjustChkTime = tChkTime - iSplitTotalSec;
    time_t tAdjustCurSecond = GetCurSecond() - iSplitTotalSec;

    int32_t iChkHour = GetHourInDay(tAdjustChkTime);
    int32_t iCurHour = GetHourInDay(tAdjustCurSecond);

    return (iChkHour == iCurHour);
}

//根据tBaseTime所有当天，算出当天iSplitTime刷新时间所在的time_t时间
//比如iSplitTime是0500，表示每天5:00刷新，tBaseTime是昨天15：00对应的Unix时间戳，则返回昨天5:00的Unix时间戳
int64_t CZoneTime::ChgSplitTime2Sys(int32_t iSplitTime, time_t tBaseTime) const
{
    int32_t iSplitHour = iSplitTime / 100;
    int32_t iSplitMin = iSplitTime % 100;

    int64_t illLocalBase = ConvSecUTCToLocal(tBaseTime);

    int64_t illBaseMidNight, illBaseSpare;
    CutSecToMidNightSpare(illLocalBase, illBaseMidNight, illBaseSpare);

    return  ConvSecLocalToUTC(illBaseMidNight + iSplitHour * ONE_HOUR_SECONDS + iSplitMin * ONE_MIN_SECONDS);

    /*
    int32_t iSplitHour = iSplitTime / 100;
    int32_t iSplitMin = iSplitTime % 100;

    struct tm stCurTime;
    localtime_r(&tBaseTime, &stCurTime);

    stCurTime.tm_hour = iSplitHour;
    stCurTime.tm_min = iSplitMin;
    stCurTime.tm_sec = 0;

    return (int32_t)mktime(&stCurTime);
    */
}

BOOL CZoneTime::ChkGameInSameWeek(int64_t illTime1, int64_t illTime2)
{
    int32_t iWeek1 = GetWeekNo(illTime1);
    int32_t iWeek2 = GetWeekNo(illTime2);

    return (iWeek1 == iWeek2);
}

BOOL CZoneTime::GM_SetOffsetSec(int32_t iOffsetSec)
{
    m_iGMOffsetSec = iOffsetSec;
    UpdateTime();
    return true;
}

const ST_DATE& CZoneTime::GetCurDate(BOOL bRealTime)
{
    tm stSysDate;
    time_t tCurTime = GetCurSecond(bRealTime);
    localtime_r(&tCurTime, &stSysDate);

    static ST_DATE s_stDate;
    s_stDate.Reset();
    s_stDate.m_iYear = stSysDate.tm_year + 1900;
    s_stDate.m_iMonth = stSysDate.tm_mon + 1;
    s_stDate.m_iDay = stSysDate.tm_mday;
    s_stDate.m_iHour = stSysDate.tm_hour;
    s_stDate.m_iMin = stSysDate.tm_min;
    s_stDate.m_iSec = stSysDate.tm_sec;

    return s_stDate;
}

ST_DATE& CZoneTime::_GetCurDate()
{
    tm stSysDate;
    time_t tCurTime = GetCurSecond();
    localtime_r(&tCurTime, &stSysDate);

    static ST_DATE s_stDate;
    s_stDate.Reset();
    s_stDate.m_iYear = stSysDate.tm_year + 1900;
    s_stDate.m_iMonth = stSysDate.tm_mon + 1;
    s_stDate.m_iDay = stSysDate.tm_mday;
    s_stDate.m_iHour = stSysDate.tm_hour;
    s_stDate.m_iMin = stSysDate.tm_min;
    s_stDate.m_iSec = stSysDate.tm_sec;

    return s_stDate;
}

const char* CZoneTime::GetCurDateStr()
{
    //同一个log内，连续调用GetCurDateStr时，static会返回同一个时间字符串
    //使用下面的技术，可以连续调用
    const int STR_ARRAY_SIZE = 10;
    static char s_szCurTimeStr[STR_ARRAY_SIZE][MAX_TIME_STR_LENGTH] = { "", };
    static uint16_t s_StrIdx = 0;

    char* psCurStr = s_szCurTimeStr[s_StrIdx % STR_ARRAY_SIZE];
    s_StrIdx++;

    const ST_DATE& rstDate = GetCurDate();

    snprintf(psCurStr, MAX_TIME_STR_LENGTH, "%04d-%02d-%02d %02d:%02d:%02d",
        rstDate.m_iYear, rstDate.m_iMonth, rstDate.m_iDay,
        rstDate.m_iHour, rstDate.m_iMin, rstDate.m_iSec);

    return psCurStr;
}

const char* CZoneTime::GetYYYYMMDDStr()
{
    const int STR_ARRAY_SIZE = 10;
    static char s_szCurTimeStr[STR_ARRAY_SIZE][MAX_TIME_STR_LENGTH] = { "", };
    static uint16_t s_StrIdx = 0;

    char* psCurStr = s_szCurTimeStr[s_StrIdx % STR_ARRAY_SIZE];
    s_StrIdx++;

    const ST_DATE& rstDate = GetCurDate();

    snprintf(psCurStr, MAX_TIME_STR_LENGTH, "%04d-%02d-%02d",
        rstDate.m_iYear, rstDate.m_iMonth, rstDate.m_iDay);

    return psCurStr;
}

const char* CZoneTime::GetYYYYMMDDStr(int64_t illTime)
{
    const int STR_ARRAY_SIZE = 10;
    static char s_szCurTimeStr[STR_ARRAY_SIZE][MAX_TIME_STR_LENGTH] = { "", };
    static uint64_t ullStrIdx = 0;

    char* psCurStr = s_szCurTimeStr[ullStrIdx % STR_ARRAY_SIZE];
    ullStrIdx++;

    const ST_DATE& rstDate = ConvertSecToDate(illTime);

    snprintf(psCurStr, MAX_TIME_STR_LENGTH, "%04d-%02d-%02d",
        rstDate.m_iYear, rstDate.m_iMonth, rstDate.m_iDay);

    return psCurStr;
}

const char* CZoneTime::GetYYYYMMDDHHStr(BOOL bRealTime)
{
    const int STR_ARRAY_SIZE = 10;
    static char s_szCurTimeStr[STR_ARRAY_SIZE][MAX_TIME_STR_LENGTH] = { "", };
    static uint64_t ullStrIdx = 0;

    char* psCurStr = s_szCurTimeStr[ullStrIdx % STR_ARRAY_SIZE];
    ullStrIdx++;

    const ST_DATE& rstDate = GetCurDate(bRealTime);

    snprintf(psCurStr, MAX_TIME_STR_LENGTH, "%04d%02d%02d%02d",
        rstDate.m_iYear, rstDate.m_iMonth, rstDate.m_iDay, rstDate.m_iHour);

    return psCurStr;
}

const char* CZoneTime::GetYYYYMMDDHHMMSSStr(int64_t illTime)
{
    const int STR_ARRAY_SIZE = 10;
    static char s_szCurTimeStr[STR_ARRAY_SIZE][MAX_TIME_STR_LENGTH] = { "", };
    static uint64_t ullStrIdx = 0;

    char* psCurStr = s_szCurTimeStr[ullStrIdx % STR_ARRAY_SIZE];
    ullStrIdx++;

    const ST_DATE& rstDate = ConvertSecToDate(illTime);

    snprintf(psCurStr, MAX_TIME_STR_LENGTH, "%04d%02d%02d %02d:%02d:%02d",
        rstDate.m_iYear, rstDate.m_iMonth, rstDate.m_iDay, rstDate.m_iHour, rstDate.m_iMin, rstDate.m_iSec);

    return psCurStr;
}

const char* CZoneTime::GetDateStr(int64_t illTime)
{
    //同一个log内，连续调用GetDateStr时，static会返回同一个时间字符串
    //使用下面的技术，可以连续调用
    const int STR_ARRAY_SIZE = 10;
    static char s_szCurTimeStr[STR_ARRAY_SIZE][MAX_TIME_STR_LENGTH] = { "", };
    static uint64_t ullStrIdx = 0;

    char* psCurStr = s_szCurTimeStr[ullStrIdx % STR_ARRAY_SIZE];
    ullStrIdx++;

    const ST_DATE& rstDate = ConvertSecToDate(illTime);

    snprintf(psCurStr, MAX_TIME_STR_LENGTH, "%04d-%02d-%02d %02d:%02d:%02d",
        rstDate.m_iYear, rstDate.m_iMonth, rstDate.m_iDay,
        rstDate.m_iHour, rstDate.m_iMin, rstDate.m_iSec);

    return psCurStr;
}

int64_t CZoneTime::GetSTDateLong(int64_t illSeconds) {
    tm stDate;
    time_t tSecond = illSeconds;
    localtime_r(&tSecond, &stDate);
    return (uint64_t)stDate.tm_sec + (uint64_t)stDate.tm_min * 100L
        + (uint64_t)stDate.tm_hour * 10000L + (uint64_t)stDate.tm_mday * 1000000L
        + (uint64_t)(stDate.tm_mon + 1) * 100000000L + (uint64_t)(stDate.tm_year + 1900L) * 10000000000L;
}

const ST_DATE& CZoneTime::ConvertSecToDate(int64_t illTime)
{
    tm stSysDate;
    time_t tCurTime = illTime;
    localtime_r(&tCurTime, &stSysDate);

    static ST_DATE s_stDate;
    s_stDate.Reset();
    s_stDate.m_iYear = stSysDate.tm_year + 1900;
    s_stDate.m_iMonth = stSysDate.tm_mon + 1;
    s_stDate.m_iDay = stSysDate.tm_mday;
    s_stDate.m_iHour = stSysDate.tm_hour;
    s_stDate.m_iMin = stSysDate.tm_min;
    s_stDate.m_iSec = stSysDate.tm_sec;

    return s_stDate;
}

ST_DATE& CZoneTime::_ConvertSecToDate(uint32_t dwTime)
{
    tm stSysDate;
    time_t tCurTime = dwTime;
    localtime_r(&tCurTime, &stSysDate);

    static ST_DATE s_stDate;
    s_stDate.Reset();
    s_stDate.m_iYear = stSysDate.tm_year + 1900;
    s_stDate.m_iMonth = stSysDate.tm_mon + 1;
    s_stDate.m_iDay = stSysDate.tm_mday;
    s_stDate.m_iHour = stSysDate.tm_hour;
    s_stDate.m_iMin = stSysDate.tm_min;
    s_stDate.m_iSec = stSysDate.tm_sec;

    return s_stDate;
}

int64_t CZoneTime::ConvertDate2Sec(const ST_DATE& rstDate)
{
    tm stSysDate = { 0 };
    stSysDate.tm_year = rstDate.m_iYear - 1900;
    stSysDate.tm_mon = rstDate.m_iMonth - 1;
    stSysDate.tm_mday = rstDate.m_iDay;
    stSysDate.tm_hour = rstDate.m_iHour;
    stSysDate.tm_min = rstDate.m_iMin;
    stSysDate.tm_sec = rstDate.m_iSec;

    return mktime(&stSysDate);
}

int64_t CZoneTime::ConvertDate2Sec(int64_t illDate)
{
    const ST_DATE& rstDate = ConvertDate(illDate);
    return ConvertDate2Sec(rstDate);
}

int32_t CZoneTime::ConvertTime2Sec(int64_t llTime)
{
    if (llTime < 0)
    {
        //LOGERR(ZERO_UID, MODULE_TIME, "Invalid input@llTIme<%ld>", llTime);
        return 0;
    }

    int32_t iSec = llTime % 100;

    llTime /= 100;
    int32_t iMin = llTime % 100;

    llTime /= 100;
    int32_t iHour = llTime % 100;

    return iHour * ONE_HOUR_SECONDS + iMin * ONE_MIN_SECONDS + iSec;
}

// 格式 20150501123000，不设置日期，就取当前日期
const ST_DATE& CZoneTime::ConvertDate(int64_t illDate)
{
    ST_DATE& stDate = _GetCurDate();

    stDate.m_iSec = illDate % 100;

    illDate /= 100;
    stDate.m_iMin = illDate % 100;

    illDate /= 100;
    stDate.m_iHour = illDate % 100;

    illDate /= 100;
    if (illDate > 0)
    {
        stDate.m_iDay = illDate % 100;

        illDate /= 100;
        stDate.m_iMonth = illDate % 100;

        illDate /= 100;
        stDate.m_iYear = illDate % 10000;
    }

    return stDate;
}

int32_t CZoneTime::GetWDay(int64_t illTime) const
{
    int64_t illLocalTime = ConvSecUTCToLocal(illTime);

    // 1970-1-1是周4
    int32_t dwDays = (int32_t)(illLocalTime / ONE_DAY_SECONDS);
    return ((dwDays + 4) % 7);

    /*
    time_t tTime = dwTime;
    struct tm Tm;

    localtime_r(&tTime,&Tm);

    return    Tm.tm_wday;
    */
}

int32_t CZoneTime::GetSecToNextSplitTime(int32_t iSplitTime)
{
    int64_t illCurTimeSec = GetCurSecond();
    int64_t illSplitTime = ChgSplitTime2Sys(iSplitTime, illCurTimeSec);
    int64_t illMinSplitTime = 0;
    (void)illMinSplitTime; // 消除warning
    int64_t illMaxSplitTime = 0;

    if (illSplitTime <= illCurTimeSec)
    {
        illMinSplitTime = illSplitTime;
        illMaxSplitTime = illSplitTime + ONE_DAY_SECONDS;
    }
    else
    {
        illMinSplitTime = illSplitTime - ONE_DAY_SECONDS;
        illMaxSplitTime = illSplitTime;
    }

    return (illMaxSplitTime - illCurTimeSec);
}

int32_t CZoneTime::CalcPassedMonth(int64_t illStartSec, int64_t illEndSec)
{
    if (illStartSec >= illEndSec)
    {
        return 0;
    }

    const ST_DATE& rstStartDate = ConvertSecToDate(illStartSec);
    int32_t iStartMonth = rstStartDate.m_iYear * 12 + rstStartDate.m_iMonth;

    const ST_DATE& rstEndDate = ConvertSecToDate(illEndSec);
    int32_t iEndMonth = rstEndDate.m_iYear * 12 + rstEndDate.m_iMonth;

    if (iStartMonth >= iEndMonth)
    {
        return 0;
    }

    return iEndMonth - iStartMonth;
}

int32_t CZoneTime::CalcPassedDay(int64_t illStartSec, int64_t illEndSec)
{
    if (illStartSec >= illEndSec)
    {
        return 0;
    }

    ST_DATE& rstStartDate = _ConvertSecToDate(illStartSec);

    rstStartDate.m_iHour = 0;
    rstStartDate.m_iMin = 0;
    rstStartDate.m_iSec = 0;
    int64_t illStartSecZero = ConvertDate2Sec(rstStartDate);

    ST_DATE& rstEndDate = _ConvertSecToDate(illEndSec);
    rstEndDate.m_iHour = 0;
    rstEndDate.m_iMin = 0;
    rstEndDate.m_iSec = 0;
    int64_t illEndSecZero = ConvertDate2Sec(rstEndDate);

    if (illStartSecZero >= illEndSecZero)
    {
        return 0;
    }

    return (illEndSecZero - illStartSecZero) / ONE_DAY_SECONDS;
}

BOOL CZoneTime::CheckDateFormatValid(int64_t ilDate, ST_DATE& stDate)
{
    if (!CheckTimeFormatValid(ilDate, stDate))
    {
        return false;
    }

    ilDate /= 1000000;
    stDate.m_iDay = ilDate % 100;

    ilDate /= 100;
    stDate.m_iMonth = ilDate % 100;

    ilDate /= 100;
    stDate.m_iYear = ilDate % 10000;

    ilDate /= 10000;
    if (ilDate > 0)
    {
        return false;
    }

    if (stDate.m_iYear < 2015
        || stDate.m_iMonth < 1 || stDate.m_iMonth>12
        || stDate.m_iDay < 1 || stDate.m_iDay>31)
    {
        return false;
    }

    switch (stDate.m_iMonth)
    {
    case 2:
    {
        if (((stDate.m_iYear % 4 == 0) && (stDate.m_iYear % 100 != 0)) || (stDate.m_iYear % 400 == 0))
        {
            if (stDate.m_iDay > 29)
            {
                return false;
            }
        }
        else
        {
            if (stDate.m_iDay > 28)
            {
                return false;
            }
        }
    }
    break;

    case 4:
    case 6:
    case 9:
    case 11:
    {
        if (stDate.m_iDay > 30)
        {
            return false;
        }
    }
    break;

    default:
        break;
    }

    return true;
}

BOOL    CZoneTime::CheckTimeFormatValid(int64_t ilTime, ST_DATE& stDate)
{
    if (ilTime == 0)
    {
        stDate.m_iHour = 0;
        stDate.m_iMin = 0;
        stDate.m_iSec = 0;
        return true;
    }

    // 秒
    int32_t iSec = ilTime % 100;
    if (iSec > 59)
    {
        return false;
    }

    // 分
    ilTime /= 100;
    int32_t iMin = ilTime % 100;
    if (iMin > 59)
    {
        return false;
    }

    // 时
    ilTime /= 100;
    int32_t iHour = ilTime % 100;
    if (iHour > 23)
    {
        return false;
    }

    stDate.m_iHour = iHour;
    stDate.m_iMin = iMin;
    stDate.m_iSec = iSec;

    return true;
}

int64_t CZoneTime::ChgStartDate2Sec(int32_t iDate)
{
    int32_t iYear = iDate / 10000;

    iDate = iDate % 10000;
    int32_t iMonth = iDate / 100;

    int32_t iDay = iDate % 100;

    tm Tm = { 0 };
    Tm.tm_year = iYear - 1900;
    Tm.tm_mon = iMonth - 1;
    Tm.tm_mday = iDay;
    Tm.tm_hour = 0;
    Tm.tm_min = 0;
    Tm.tm_sec = 0;

    return mktime(&Tm);
}

int64_t CZoneTime::ChgEndDate2Sec(int32_t iDate)
{
    int32_t iYear = iDate / 10000;

    iDate = iDate % 10000;
    int32_t iMonth = iDate / 100;

    int32_t iDay = iDate % 100;

    tm Tm = { 0 };
    Tm.tm_year = iYear - 1900;
    Tm.tm_mon = iMonth - 1;
    Tm.tm_mday = iDay;
    Tm.tm_hour = 23;
    Tm.tm_min = 59;
    Tm.tm_sec = 59;

    return mktime(&Tm);
}

int32_t CZoneTime::ChgTime2SecInDay(int32_t iTime)
{
    int32_t iHour = iTime / 10000;

    iTime = iTime % 10000;
    int32_t iMin = iTime / 100;

    int32_t iSec = iTime % 100;

    return (iHour * ONE_HOUR_SECONDS + iMin * ONE_MIN_SECONDS + iSec);
}

int64_t CZoneTime::ChgTime2Sec(int32_t iTime, const ST_DATE* pDate)
{
    if (!pDate)
    {
        pDate = &GetCurDate();
    }

    tm Tm = { 0 };
    Tm.tm_year = pDate->m_iYear - 1900;
    Tm.tm_mon = pDate->m_iMonth - 1;
    Tm.tm_mday = pDate->m_iDay;
    Tm.tm_hour = iTime / 10000;
    Tm.tm_min = iTime % 10000 / 100;
    Tm.tm_sec = iTime % 100;

    return mktime(&Tm);
}

BOOL CZoneTime::ChkDateFormatValid(int32_t iDate)
{
    int32_t iYear = iDate / 10000;

    iDate = iDate % 10000;
    int32_t iMonth = iDate / 100;

    int32_t iDay = iDate % 100;

    if (iYear < 2015 || iMonth < 1 || iMonth > 12 || iDay < 1 || iDay > 31)
    {
        return false;
    }

    switch (iMonth)
    {
    case 2:
    {
        if ((iYear % 4 == 0 && iYear % 100 != 0) || iYear % 400 == 0)
        {
            if (iDay > 29)
            {
                return false;
            }
        }
        else
        {
            if (iDay > 28)
            {
                return false;
            }
        }
    }
    break;

    case 4:
    case 6:
    case 9:
    case 11:
    {
        if (iDay > 30)
        {
            return false;
        }
    }
    break;
    }

    return true;
}

BOOL CZoneTime::ChkTimeFormatValid(int32_t iTime)
{
    int32_t iHour = iTime / 10000;

    iTime = iTime % 10000;
    int32_t iMin = iTime / 100;

    int32_t iSec = iTime % 100;

    return (iHour <= 23 && iMin <= 59 && iSec <= 59);
}

void CZoneTime::SetCfgUpdateTimeAlarmThreshold(int32_t iCfgUpdateTimeAlarmThreshold)
{
    if (0 == iCfgUpdateTimeAlarmThreshold)
    {
        //LOGERR(ZERO_UID, MODULE_FRAMEWORK, "SetCfgUpdateTimeAlarmThreshold cfg is zero");
        return;
    }
    m_iCfgUpdateTimeAlarmThreshold = iCfgUpdateTimeAlarmThreshold;
    //LOGRUN(ZERO_UID, MODULE_FRAMEWORK, "SetCfgUpdateTimeAlarmThreshold %d", m_iCfgUpdateTimeAlarmThreshold);
}

// 获得当前时间对应的TimeStr
char* CZoneTime::GetCurDateTimeStr()
{
    // 如果这秒还没有更新，更新下
    if (GetCurSecond(false) != m_illTimeStrSec)
    {
        // 更新下时间戳
        time_t iCurSecond = CZoneTime::Instance().GetCurSecond();
        m_illTimeStrSec = iCurSecond;

        // 更新下当前秒对应的DataStrBuff
        char* szCurTimeStr = DateTimeStr(&iCurSecond);
        StrCpy(m_szCurTimeStr, szCurTimeStr, sizeof(m_szCurTimeStr));
    }

    // 返回对应的DataTimeStr
    return m_szCurTimeStr;
}

int64_t CZoneTime::ConvStrDateToSec(const std::string& strDate, BOOL bIsBeijing)
{
    if (strDate.empty())
    {
        return 0;
    }
    struct tm stTm;
    memset(&stTm, 0, sizeof(stTm));
    if (NULL == tdr_pal_strptime(strDate.c_str(), "%Y-%m-%d %H:%M:%S", &stTm))
    {
        //LOGERR(ZERO_UID, MODULE_FRAMEWORK, "ConvStrDateToSec is error, strDate:%s", strDate.c_str());
        return 0;
    }

    int64_t illTime = mktime(&stTm);
    if (bIsBeijing)
    {
        /*北京时间2022-03-01 10:00:10（UTC时间是 2022-03-01 02:00:10）
        使用tdr_pal_strptime后，时间变成UTC时间2022-03-01 10:00:10（北京时间2022-03-01 18:00:10）
        所以要减去8个小时
        */
        illTime = illTime - 8 * ONE_HOUR_SECONDS;
    }
    return illTime;

}

BOOL CZoneTime::ChkInTimePeriod(int64_t illTime, int64_t illStartTime, int64_t illEndTime)
{
    if (0 == illStartTime && 0 == illEndTime)
    {
        return false;
    }

    if (0 == illStartTime && 0 < illEndTime)
    {
        if (illTime < illEndTime)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (0 < illStartTime && 0 == illEndTime)
    {
        if (illTime > illStartTime)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (illTime > illStartTime && illTime < illEndTime)
    {
        return true;
    }

    return false;
}

int32_t CZoneTime::_GetTimeZoneMinute()
{
    time_t dwCurTime;
    struct tm* stTimeInfo;

    time(&dwCurTime);
    stTimeInfo = gmtime(&dwCurTime);
    if (NULL == stTimeInfo)
    {
        return 0;
    }
    time_t dwUtcTime = mktime(stTimeInfo);

    stTimeInfo = localtime(&dwCurTime);
    if (NULL == stTimeInfo)
    {
        return 0;
    }
    time_t dwLocalTime = mktime(stTimeInfo);

    double fOffsetFromUTC = difftime(dwUtcTime, dwLocalTime) / ONE_MIN_SECONDS;

    //     夏令时活动可能出问题，这个不判断(夏令时扣60分钟)
    //     if (stTimeInfo->tm_isdst)
    //     {
    //         fOffsetFromUTC -= 60;
    //     }

    return (int)fOffsetFromUTC;
}

// 返回当天的第二天的0点时间戳
int64_t CZoneTime::GetNextDayZeroTime()
{
    int64_t llCurTime = GetCurSecond();
    return llCurTime - GetSecInDay(llCurTime) + ONE_DAY_SECONDS;
}

// 返回下一周的0点时间戳
int64_t CZoneTime::GetNextWeekZeroTime()
{
    int64_t llCurTime = GetCurSecond();
    // 当天剩余时间(不包含当天)
    int64_t llDayResidueTIme = GetNextDayZeroTime() - llCurTime;
    // 这周还剩余多少天
    int32_t  iResidueDay = DAY_NUM_OF_ONE_WEEK - GetWDay(llCurTime);
    return llCurTime + llDayResidueTIme + (iResidueDay * ONE_DAY_SECONDS);
}

// 返回下个月的0点时间戳
int64_t CZoneTime::GetNextMonthZeroTime()
{
    int64_t llCurTime = GetCurSecond();
    int32_t iTotalDays = GetTotalDaysInCurrentMonth(llCurTime);
    // 当天剩余时间
    int64_t llDayResidueTIme = GetNextDayZeroTime() - llCurTime;
    // 这月还剩余多少天(不包含当天)
    int32_t  iResidueDay = iTotalDays - GetDayInMonthSplitTime(0);
    return llCurTime + llDayResidueTIme + iResidueDay * ONE_DAY_SECONDS;
}

CPerfPrinter::CPerfPrinter(const std::string& strTitle)
    : m_szTitle(strTitle), m_llBegin(0), m_llEnd(0)
{
    TIME_VAL stTV;
    gettimeofday(&stTV, NULL);
    m_llBegin = stTV.tv_sec * 1000 + stTV.tv_usec / 1000;
    //LOGSTAT("%s begin at %ld MS", m_strTitle.c_str(), m_llBegin);
}

CPerfPrinter::~CPerfPrinter()
{
    TIME_VAL stTV;
    gettimeofday(&stTV, NULL);
    m_llEnd = stTV.tv_sec * 1000 + stTV.tv_usec / 1000;
    //LOGSTAT("%s end at %ld MS, spend %ld MS", m_strTitle.c_str(), m_llBegin, m_llEnd - m_llBegin);
}