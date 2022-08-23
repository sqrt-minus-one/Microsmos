/* date = July 18th 2022 4:07 pm */

#ifndef LINUX_TIME_H
#define LINUX_TIME_H

#define IMPL_GetCurrentTimeMicroSeconds  Linux_GetCurrentTimeMicroSeconds
#define IMPL_GetCurrentDateTime          Linux_GetCurrentDateTime
#define IMPL_GetCurrentTime              Linux_GetCurrentTime
#define IMPL_GetDateTimeFromTime         Linux_GetDateTimeFromTime
#define IMPL_GetTimeFromDateTime         Linux_GetTimeFromDateTime


internal os_time      Linux_GetCurrentTimeMicroSeconds();
internal os_data_time Linux_GetCurrentDateTime();
internal os_time      Linux_GetCurrentTime();

internal os_data_time Linux_GetDateTimeFromTime(os_time);
internal os_time      Linux_GetTimeFromDateTime(os_data_time);

#endif //LINUX_TIME_H
