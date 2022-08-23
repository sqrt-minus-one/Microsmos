/* date = July 18th 2022 3:54 pm */

#ifndef OS_TIME_H
#define OS_TIME_H

typedef enum os_month os_month;
enum os_month
{
    month_January,
    month_February,
    month_March,
    month_April,
    month_May,
    month_June,
    month_July,
    month_August,
    month_September,
    month_October,
    month_November,
    month_December,
    month_Count,
};


typedef u64 os_time;

typedef struct os_data_time os_data_time;
struct os_data_time
{
    u32 Year;
    os_month Month;
    u8 Day;
    u8 Hour;
    u8 Minute;
    u8 Second;
    u16 MiliSecond;
};

read_only global string8 MonthFullName[month_Count] = {
    [month_January]   = Str8LitComp("January"),
    [month_February]  = Str8LitComp("February"),
    [month_March]     = Str8LitComp("March"),
    [month_April]     = Str8LitComp("April"),
    [month_May]       = Str8LitComp("May"),
    [month_June]      = Str8LitComp("June"),
    [month_July]      = Str8LitComp("July"),
    [month_August]    = Str8LitComp("August"),
    [month_September] = Str8LitComp("September"),
    [month_October]   = Str8LitComp("October"),
    [month_November]  = Str8LitComp("November"),
    [month_December]  = Str8LitComp("December"),
};

read_only global string8 MonthAbbrevdName[month_Count] = {
    [month_January]   = Str8LitComp("Jan"),
    [month_February]  = Str8LitComp("Feb"),
    [month_March]     = Str8LitComp("Mar"),
    [month_April]     = Str8LitComp("Apr"),
    [month_May]       = Str8LitComp("May"),
    [month_June]      = Str8LitComp("Jun"),
    [month_July]      = Str8LitComp("Jul"),
    [month_August]    = Str8LitComp("Aug"),
    [month_September] = Str8LitComp("Sep"),
    [month_October]   = Str8LitComp("Oct"),
    [month_November]  = Str8LitComp("Nov"),
    [month_December]  = Str8LitComp("Dec"),
};

internal os_time OS_GetCurrentTimeMicroSeconds();

internal os_data_time OS_GetCurrentDateTime();
internal os_time      OS_GetCurrentTime();

internal os_data_time OS_GetDateTimeFromTime(os_time);
internal os_time      OS_GetTimeFromDateTime(os_data_time);

#endif //OS_TIME_H
