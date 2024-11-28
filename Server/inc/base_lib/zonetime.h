#ifndef ZONETIME_H
#define ZONETIME_H

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <assert.h>
#include    <mutex>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define localtime_r(a,b) localtime_s(b,a)
#else
#include    <sys/time.h>
#include    <sys/timeb.h>
#endif// defined(_WIN32) || defined(_WIN64)
#include    <cstdint>
#include    <intrin.h>
#include    <time.h>
#include    <string>

#define ONE_YEAR_SECONDS          (31536000)  // 一年的秒数
#define ONE_MONTH_SECONDS         (2592000)   // 一个月折算成秒数，只算30天
#define ONE_WEEK_SECONDS          (604800)    // 一周折算成秒数
#define ONE_DAY_SECONDS           (86400)     // 一天折算成秒数
#define ONE_HOUR_SECONDS          (3600)      // 一小时的秒数
#define ONE_MIN_SECONDS           (60)
#define ONE_MIN_MSEC              (60000)     // 一分钟有多少毫秒
#define ONE_DAY_HOURS             (24)
#define ONE_DAY_MINS              (1440)      //一天分钟数
#define ONE_YEAR_MONTHS           (12)        // 一年的月数
#define USEC_NUM_PER_SECOND       (1000000)   //每秒钟有多少tv_usec
#define MSEC_NUM_PER_SECOND       (1000)      //每秒有多少毫秒
#define NANOSEC_NUM_PER_MSECOND   (1000000)   //每毫秒有多少纳秒
#define MILL_SECONDS_PER_TV_SEC   (1000)      //tv_sec表示多少毫秒
#define USEC_NUM_PERMILLSECOND    (1000)      //每毫秒有多少tv_usec
#define DAY_NUM_OF_ONE_WEEK       (7)         //一个星期的天数，具体星期1-6分别为对应数值，而星期日为0
#define WEEK_DAY_OF_UTC_BEGIN_DAY (4)         //time函数起始时间的星期数，即1970.1.1日为星期四
#define MAX_TIME_STR_LENGTH       (50)
#define ONE_HUNDRED               (100)
#define ONE_HOUR_MIN              (60)        // 一小时分钟数

#define MAX_CUR_TIME_LEN           (64)
#define MAX_PRINTER_LEN            (64)

//----------------------
#define COM_SECTIMESTAMP_AT_2020   (1577808000) // 2020年1月1日0点的时间戳秒数

typedef struct timeval TIME_VAL;


struct ST_DATE
{
    int32_t m_iYear;
    int32_t m_iMonth;
    int32_t m_iDay;
    int32_t m_iHour;
    int32_t m_iMin;
    int32_t m_iSec;

    void Reset()
    {
        m_iYear  = 0;
        m_iMonth = 0;
        m_iDay   = 0;
        m_iHour  = 0;
        m_iMin   = 0;
        m_iSec   = 0;
    }

    ST_DATE()
    {
        Reset();
    }
};



class CZoneTime
{
public:
    CZoneTime();
    ~CZoneTime();

public:
    static CZoneTime* getInstance();
    static void releaseInstance();
    static std::mutex mutex;

    void UpdateTime(BOOL bCheckModifyTime = true);

    const TIME_VAL& GetCurTime(BOOL bRealTime = false);

    // 返回当前的秒数
    int64_t         GetCurSecond(BOOL bRealTime = false);
    // 返回当前和20200101000000（2020年1月1号）相差的时间秒数
    int32_t         GetOffsetSecOf2020(BOOL bRealTime = false);
    // 通过当前和20200101000000（2020年1月1号）相差的时间秒数，返回当前的秒数
    int64_t         GetSecByOffsetSecOf2020(int32_t iOffsetSece);
    int64_t         GetCurLocalSecond(BOOL bRealTime = false);
    // 返回当前timeval.tv_usec转换后的微秒
    int32_t         GetCurUSecond(BOOL bRealTime = false);
    // 返回当前timeval.tv_usec转换后的毫秒
    int32_t         GetCurMSecond(BOOL bRealTime = false);

    // 返回当前的毫秒数
    int64_t         GetCurMS(BOOL bRealTime = false);

    int32_t         GetCurIDInSecond();

    uint64_t        GetCpuTickCnt();

    int32_t         GetTimeZoneSec() { return m_iTimeZoneSec; }
    int32_t         GetTimeZoneSecOppositeNum(BOOL bRealTime = false);

    // 返回当前时间是一天中第几个小时
    int32_t         GetHourInDay(int64_t illTime);
    // 返回当前时间是一天中第几分钟
    int32_t         GetMinInHour(int64_t illTime);
    // 返回当前时间是一天中第几秒
    int32_t         GetSecInDay(int64_t illTime);
    // 返回第二天的0
    // 返回当前0点时间戳
    int64_t GetZeroTimeOfDay(int64_t llTime);
    // 返回当天的第二天的0点时间戳
    int64_t GetNextDayZeroTime();
    // 返回下一周的0点时间戳
    int64_t GetNextWeekZeroTime();
    // 返回下个月的0点时间戳
    int64_t GetNextMonthZeroTime();
    // 比如iSplitTime是0500，表示每天5:00刷新；以SplitTime分割，计算当前是一个月中第几天
    int32_t         GetDayInMonthSplitTime(int32_t iSplitTime);
    // 比如iSplitTime是0500，表示每天5:00刷新；以SplitTime分割，计算当前是第几月
    int32_t         GetMonthSplitTime(int32_t iSplitTime);

    // 获取当前秒数所在的月有多少天
    int32_t         GetTotalDaysInCurrentMonth(int64_t illSec);

    // 计算illNow所在天的0点到illLast所在天的0点经过多少天，illNow较小时返回0
    int32_t         CalAlternateDays(int64_t illNow, int64_t illLast);

    // 计算本周周一SplieTime的秒数，以星期一开始一周
    int64_t         GetMondaySplitTime(int32_t iSplitTime);
    int64_t         GetMondaySplitTime(int32_t iSplitTime, int64_t llNow) const;
    int64_t         GetLastMondaySplitTime(int32_t iSplitTime);

    // 比如iSplitTime是0500，表示每天5:00刷新，返回tChkTime和当前时间是否根据在同一天内(同一天是指从5：00到次日05:00)
    BOOL            ChkGameInSameDay(int32_t iSplitTime, time_t tChkTime);

    // 比如iSplitTime是0500，表示每天5:00刷新，返回tTime1和tTime2是否根据在同一天内(同一天是指从5：00到次日05:00)
    BOOL            ChkGameInSameDay(int32_t iSplitTime, time_t tTime1, time_t tTime2);

    // 比如iSplitTime是0500，表示每天5:00刷新，返回tChkTime和当前时间是否根据在同一个月内(同一月是指从本月第一天5：00到下月第一天的05:00)
    BOOL            ChkGameInSameMonth(int32_t iSplitTime, time_t tChkTime);

    //比如iSplitMin是2，iSplitSec是10，表示每小时开始的02:10时刷新
    BOOL            ChkGameInSameHour(int32_t iSplitMin, int32_t iSplitSec, time_t tChkTime);


    //根据tBaseTime所有当天，算出当天iSplitTime刷新时间所在的time_t时间
    //比如iSplitTime是0500，表示每天5:00刷新，tBaseTime是昨天15：00，则返回昨天5:00的time_t时间
    int64_t         ChgSplitTime2Sys(int32_t iSplitTime, time_t tBaseTime) const;

    BOOL            ChkGameInSameWeek(int64_t illTime1, int64_t illTime2);

    BOOL            GM_SetOffsetSec(int32_t iOffsetSec);

    const ST_DATE& GetCurDate(BOOL bRealTime = false);
    const char* GetCurDateStr();
    const char* GetYYYYMMDDStr();
    const char* GetYYYYMMDDStr(int64_t illTime);
    const char* GetYYYYMMDDHHStr(BOOL bRealTime = false);
    const char* GetYYYYMMDDHHMMSSStr(int64_t illTime);
    static const char* GetDateStr(int64_t illTime);
    const ST_DATE& ConvertDate(int64_t illDate);
    int64_t         ConvertDate2Sec(const ST_DATE& rstDate);
    int64_t         GetSTDateLong(int64_t illSeconds);

    // illDate格式：20150501123000；illDate格式的时间转化成秒
    int64_t         ConvertDate2Sec(int64_t illDate);

    // llTime格式： 235959； llTime转换成距离当天0点的秒数
    int32_t         ConvertTime2Sec(int64_t llTime);

    // illTime格式：秒数；illTime时间戳转换成ST_DATE结构
    static const ST_DATE& ConvertSecToDate(int64_t illTime);

    // illTime格式：秒数；illTime时间戳转换成一周的第几天
    // 星期天开始[0-6]
    int32_t         GetWDay(int64_t illTime) const;
    // 以星期一作为一周的开始
    int32_t         GetWeekNo(int64_t illTime);

    // iSpliteTime格式：0500（凌晨5点）；到下一次刷新时间iSplitTime的秒数
    int32_t         GetSecToNextSplitTime(int32_t iSplitTime);

    // 计算当前天的0点rillMidNightSec，到现在illNowSec的秒数rillSpareSec
    void            CutSecToMidNightSpare(int64_t illNowSec, int64_t& rillMidNightSec, int64_t& rillSpareSec) const;

    int64_t         ConvSecUTCToLocal(int64_t illSec) const;
    int64_t         ConvSecLocalToUTC(int64_t illSec) const;

    // 计算从illStartSec到illEndSec经历的月数
    int32_t         CalcPassedMonth(int64_t illStartSec, int64_t illEndSec);
    // 计算从illStartSec到illEndSec经历的天数
    int32_t         CalcPassedDay(int64_t illStartSec, int64_t illEndSecc);

    // ilDate格式：20190501123000；判断日期时间的正确性，正确的话转换成ST_DATA结构
    BOOL            CheckDateFormatValid(int64_t ilDate, ST_DATE& stDate);
    BOOL            CheckTimeFormatValid(int64_t ilTime, ST_DATE& stDate);

    // iDate格式：年月日20190405；返回iData日期0点的秒数
    int64_t         ChgStartDate2Sec(int32_t iDate);

    // iDate格式：年月日20190405；返回iData日期23点59分59秒的秒数
    int64_t         ChgEndDate2Sec(int32_t iDate);

    // iTime格式：时分秒103050；返回iTime时间到当天0点的秒数
    int32_t         ChgTime2SecInDay(int32_t iTime);

    // iTime格式：时分秒103050；返回pDate当前的iTime时间的秒数
    int64_t         ChgTime2Sec(int32_t iTime, const ST_DATE* pDate = NULL);

    // 检查年月日格式是否合法
    BOOL            ChkDateFormatValid(int32_t iDate);

    // 检查时分秒格式是否合法
    BOOL            ChkTimeFormatValid(int32_t iTime);

    //检查illTimeSec是否在illStartTimeSec与illEndTimeSec之间，传入的参数都是秒数
    //1. illStartTimeSec > 0 && illEndTimeSec > 0，则 illTimeSec > illStartTimeSec && illTimeSec < illEndTimeSec，返回true
    //2. illStartTimeSec == 0 && illEndTimeSec > 0，则 illTimeSec < illEndTimeSec，返回true
    //3. illStartTimeSec > 0 && illEndTimeSec == 0，则 illTimeSec > illStartTimeSec，返回true
    //4. illStartTimeSec == 0 && illEndTimeSec ==0，返回false
    BOOL            ChkInTimePeriod(int64_t illTimeSec, int64_t illStartTimeSec, int64_t illEndTimeSec);

    // 设置判定系统时间跳动的间隔秒数
    void            SetCfgUpdateTimeAlarmThreshold(int32_t iCfgUpdateTimeAlarmThreshold);

    // 获得当前时间对应的TimeStr
    char* GetCurDateTimeStr();

    //%Y-%m-%d %H:%M:%S 时间字符串转时间戳, bIsBeijing表示strDate是否是北京时间
    int64_t         ConvStrDateToSec(const std::string& strDate, BOOL bIsBeijing);

private:
    // 得到当前时区和UTC（0时区）的分钟数差值，东区为负数，西区为正数，如中国上海东8区，返回-480（分钟）。
    int32_t         _GetTimeZoneMinute();
    ST_DATE& _GetCurDate();    //返回的引用会被外部修改，仅供需要修改的函数使用
    static ST_DATE& _ConvertSecToDate(uint32_t dwTime);  //返回的引用会被外部修改，仅供需要修改的函数使用

    // 判断时间变更间隔
    void            _CheckSysTimeSkip();

    CZoneTime(const CZoneTime&) = delete;
    CZoneTime& operator=(const CZoneTime&) = delete;
private:
    TIME_VAL        m_stCurrTv;
    TIME_VAL        m_stRealTv;
    int64_t         m_illLastSecond; // 生成唯一标识的最后时间
    int32_t         m_iMaxID; // 当前秒的最大ID
    int32_t         m_iTimeZoneSec; // 时区差的秒数
    int32_t         m_iGMOffsetSec; // 时区相差的秒数
    int32_t         m_iCfgUpdateTimeAlarmThreshold; // 服务器更新时间戳的最大变更秒，如果超过这个值，则告警
    int64_t         m_illTimeStrSec; // m_szCurTimeStr对应的秒
    char            m_szCurTimeStr[MAX_CUR_TIME_LEN]; // 当前秒的%04Y-%02M-%02D %02H:%02M:%02S格式
    static CZoneTime* instance;
};


class CPerfPrinter
{
public:
    CPerfPrinter(const char* szTitle);

    ~CPerfPrinter();

private:
    char    m_szTitle[MAX_PRINTER_LEN];
    int64_t m_llBegin;
    int64_t m_llEnd;
};


int gettimeofday(struct timeval* tp, void* tzp);










#endif //ZONETIME_H