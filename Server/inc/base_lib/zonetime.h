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

#define ONE_YEAR_SECONDS          (31536000)  // һ�������
#define ONE_MONTH_SECONDS         (2592000)   // һ���������������ֻ��30��
#define ONE_WEEK_SECONDS          (604800)    // һ�����������
#define ONE_DAY_SECONDS           (86400)     // һ�����������
#define ONE_HOUR_SECONDS          (3600)      // һСʱ������
#define ONE_MIN_SECONDS           (60)
#define ONE_MIN_MSEC              (60000)     // һ�����ж��ٺ���
#define ONE_DAY_HOURS             (24)
#define ONE_DAY_MINS              (1440)      //һ�������
#define ONE_YEAR_MONTHS           (12)        // һ�������
#define USEC_NUM_PER_SECOND       (1000000)   //ÿ�����ж���tv_usec
#define MSEC_NUM_PER_SECOND       (1000)      //ÿ���ж��ٺ���
#define NANOSEC_NUM_PER_MSECOND   (1000000)   //ÿ�����ж�������
#define MILL_SECONDS_PER_TV_SEC   (1000)      //tv_sec��ʾ���ٺ���
#define USEC_NUM_PERMILLSECOND    (1000)      //ÿ�����ж���tv_usec
#define DAY_NUM_OF_ONE_WEEK       (7)         //һ�����ڵ���������������1-6�ֱ�Ϊ��Ӧ��ֵ����������Ϊ0
#define WEEK_DAY_OF_UTC_BEGIN_DAY (4)         //time������ʼʱ�������������1970.1.1��Ϊ������
#define MAX_TIME_STR_LENGTH       (50)
#define ONE_HUNDRED               (100)
#define ONE_HOUR_MIN              (60)        // һСʱ������

#define MAX_CUR_TIME_LEN           (64)
#define MAX_PRINTER_LEN            (64)

//----------------------
#define COM_SECTIMESTAMP_AT_2020   (1577808000) // 2020��1��1��0���ʱ�������

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

    // ���ص�ǰ������
    int64_t         GetCurSecond(BOOL bRealTime = false);
    // ���ص�ǰ��20200101000000��2020��1��1�ţ�����ʱ������
    int32_t         GetOffsetSecOf2020(BOOL bRealTime = false);
    // ͨ����ǰ��20200101000000��2020��1��1�ţ�����ʱ�����������ص�ǰ������
    int64_t         GetSecByOffsetSecOf2020(int32_t iOffsetSece);
    int64_t         GetCurLocalSecond(BOOL bRealTime = false);
    // ���ص�ǰtimeval.tv_usecת�����΢��
    int32_t         GetCurUSecond(BOOL bRealTime = false);
    // ���ص�ǰtimeval.tv_usecת����ĺ���
    int32_t         GetCurMSecond(BOOL bRealTime = false);

    // ���ص�ǰ�ĺ�����
    int64_t         GetCurMS(BOOL bRealTime = false);

    int32_t         GetCurIDInSecond();

    uint64_t        GetCpuTickCnt();

    int32_t         GetTimeZoneSec() { return m_iTimeZoneSec; }
    int32_t         GetTimeZoneSecOppositeNum(BOOL bRealTime = false);

    // ���ص�ǰʱ����һ���еڼ���Сʱ
    int32_t         GetHourInDay(int64_t illTime);
    // ���ص�ǰʱ����һ���еڼ�����
    int32_t         GetMinInHour(int64_t illTime);
    // ���ص�ǰʱ����һ���еڼ���
    int32_t         GetSecInDay(int64_t illTime);
    // ���صڶ����0
    // ���ص�ǰ0��ʱ���
    int64_t GetZeroTimeOfDay(int64_t llTime);
    // ���ص���ĵڶ����0��ʱ���
    int64_t GetNextDayZeroTime();
    // ������һ�ܵ�0��ʱ���
    int64_t GetNextWeekZeroTime();
    // �����¸��µ�0��ʱ���
    int64_t GetNextMonthZeroTime();
    // ����iSplitTime��0500����ʾÿ��5:00ˢ�£���SplitTime�ָ���㵱ǰ��һ�����еڼ���
    int32_t         GetDayInMonthSplitTime(int32_t iSplitTime);
    // ����iSplitTime��0500����ʾÿ��5:00ˢ�£���SplitTime�ָ���㵱ǰ�ǵڼ���
    int32_t         GetMonthSplitTime(int32_t iSplitTime);

    // ��ȡ��ǰ�������ڵ����ж�����
    int32_t         GetTotalDaysInCurrentMonth(int64_t illSec);

    // ����illNow�������0�㵽illLast�������0�㾭�������죬illNow��Сʱ����0
    int32_t         CalAlternateDays(int64_t illNow, int64_t illLast);

    // ���㱾����һSplieTime��������������һ��ʼһ��
    int64_t         GetMondaySplitTime(int32_t iSplitTime);
    int64_t         GetMondaySplitTime(int32_t iSplitTime, int64_t llNow) const;
    int64_t         GetLastMondaySplitTime(int32_t iSplitTime);

    // ����iSplitTime��0500����ʾÿ��5:00ˢ�£�����tChkTime�͵�ǰʱ���Ƿ������ͬһ����(ͬһ����ָ��5��00������05:00)
    BOOL            ChkGameInSameDay(int32_t iSplitTime, time_t tChkTime);

    // ����iSplitTime��0500����ʾÿ��5:00ˢ�£�����tTime1��tTime2�Ƿ������ͬһ����(ͬһ����ָ��5��00������05:00)
    BOOL            ChkGameInSameDay(int32_t iSplitTime, time_t tTime1, time_t tTime2);

    // ����iSplitTime��0500����ʾÿ��5:00ˢ�£�����tChkTime�͵�ǰʱ���Ƿ������ͬһ������(ͬһ����ָ�ӱ��µ�һ��5��00�����µ�һ���05:00)
    BOOL            ChkGameInSameMonth(int32_t iSplitTime, time_t tChkTime);

    //����iSplitMin��2��iSplitSec��10����ʾÿСʱ��ʼ��02:10ʱˢ��
    BOOL            ChkGameInSameHour(int32_t iSplitMin, int32_t iSplitSec, time_t tChkTime);


    //����tBaseTime���е��죬�������iSplitTimeˢ��ʱ�����ڵ�time_tʱ��
    //����iSplitTime��0500����ʾÿ��5:00ˢ�£�tBaseTime������15��00���򷵻�����5:00��time_tʱ��
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

    // illDate��ʽ��20150501123000��illDate��ʽ��ʱ��ת������
    int64_t         ConvertDate2Sec(int64_t illDate);

    // llTime��ʽ�� 235959�� llTimeת���ɾ��뵱��0�������
    int32_t         ConvertTime2Sec(int64_t llTime);

    // illTime��ʽ��������illTimeʱ���ת����ST_DATE�ṹ
    static const ST_DATE& ConvertSecToDate(int64_t illTime);

    // illTime��ʽ��������illTimeʱ���ת����һ�ܵĵڼ���
    // �����쿪ʼ[0-6]
    int32_t         GetWDay(int64_t illTime) const;
    // ������һ��Ϊһ�ܵĿ�ʼ
    int32_t         GetWeekNo(int64_t illTime);

    // iSpliteTime��ʽ��0500���賿5�㣩������һ��ˢ��ʱ��iSplitTime������
    int32_t         GetSecToNextSplitTime(int32_t iSplitTime);

    // ���㵱ǰ���0��rillMidNightSec��������illNowSec������rillSpareSec
    void            CutSecToMidNightSpare(int64_t illNowSec, int64_t& rillMidNightSec, int64_t& rillSpareSec) const;

    int64_t         ConvSecUTCToLocal(int64_t illSec) const;
    int64_t         ConvSecLocalToUTC(int64_t illSec) const;

    // �����illStartSec��illEndSec����������
    int32_t         CalcPassedMonth(int64_t illStartSec, int64_t illEndSec);
    // �����illStartSec��illEndSec����������
    int32_t         CalcPassedDay(int64_t illStartSec, int64_t illEndSecc);

    // ilDate��ʽ��20190501123000���ж�����ʱ�����ȷ�ԣ���ȷ�Ļ�ת����ST_DATA�ṹ
    BOOL            CheckDateFormatValid(int64_t ilDate, ST_DATE& stDate);
    BOOL            CheckTimeFormatValid(int64_t ilTime, ST_DATE& stDate);

    // iDate��ʽ��������20190405������iData����0�������
    int64_t         ChgStartDate2Sec(int32_t iDate);

    // iDate��ʽ��������20190405������iData����23��59��59�������
    int64_t         ChgEndDate2Sec(int32_t iDate);

    // iTime��ʽ��ʱ����103050������iTimeʱ�䵽����0�������
    int32_t         ChgTime2SecInDay(int32_t iTime);

    // iTime��ʽ��ʱ����103050������pDate��ǰ��iTimeʱ�������
    int64_t         ChgTime2Sec(int32_t iTime, const ST_DATE* pDate = NULL);

    // ��������ո�ʽ�Ƿ�Ϸ�
    BOOL            ChkDateFormatValid(int32_t iDate);

    // ���ʱ�����ʽ�Ƿ�Ϸ�
    BOOL            ChkTimeFormatValid(int32_t iTime);

    //���illTimeSec�Ƿ���illStartTimeSec��illEndTimeSec֮�䣬����Ĳ�����������
    //1. illStartTimeSec > 0 && illEndTimeSec > 0���� illTimeSec > illStartTimeSec && illTimeSec < illEndTimeSec������true
    //2. illStartTimeSec == 0 && illEndTimeSec > 0���� illTimeSec < illEndTimeSec������true
    //3. illStartTimeSec > 0 && illEndTimeSec == 0���� illTimeSec > illStartTimeSec������true
    //4. illStartTimeSec == 0 && illEndTimeSec ==0������false
    BOOL            ChkInTimePeriod(int64_t illTimeSec, int64_t illStartTimeSec, int64_t illEndTimeSec);

    // �����ж�ϵͳʱ�������ļ������
    void            SetCfgUpdateTimeAlarmThreshold(int32_t iCfgUpdateTimeAlarmThreshold);

    // ��õ�ǰʱ���Ӧ��TimeStr
    char* GetCurDateTimeStr();

    //%Y-%m-%d %H:%M:%S ʱ���ַ���תʱ���, bIsBeijing��ʾstrDate�Ƿ��Ǳ���ʱ��
    int64_t         ConvStrDateToSec(const std::string& strDate, BOOL bIsBeijing);

private:
    // �õ���ǰʱ����UTC��0ʱ�����ķ�������ֵ������Ϊ����������Ϊ���������й��Ϻ���8��������-480�����ӣ���
    int32_t         _GetTimeZoneMinute();
    ST_DATE& _GetCurDate();    //���ص����ûᱻ�ⲿ�޸ģ�������Ҫ�޸ĵĺ���ʹ��
    static ST_DATE& _ConvertSecToDate(uint32_t dwTime);  //���ص����ûᱻ�ⲿ�޸ģ�������Ҫ�޸ĵĺ���ʹ��

    // �ж�ʱ�������
    void            _CheckSysTimeSkip();

    CZoneTime(const CZoneTime&) = delete;
    CZoneTime& operator=(const CZoneTime&) = delete;
private:
    TIME_VAL        m_stCurrTv;
    TIME_VAL        m_stRealTv;
    int64_t         m_illLastSecond; // ����Ψһ��ʶ�����ʱ��
    int32_t         m_iMaxID; // ��ǰ������ID
    int32_t         m_iTimeZoneSec; // ʱ���������
    int32_t         m_iGMOffsetSec; // ʱ����������
    int32_t         m_iCfgUpdateTimeAlarmThreshold; // ����������ʱ�����������룬����������ֵ����澯
    int64_t         m_illTimeStrSec; // m_szCurTimeStr��Ӧ����
    char            m_szCurTimeStr[MAX_CUR_TIME_LEN]; // ��ǰ���%04Y-%02M-%02D %02H:%02M:%02S��ʽ
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