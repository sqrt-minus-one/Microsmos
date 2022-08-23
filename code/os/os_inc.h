/* date = June 20th 2022 6:18 pm */

#ifndef OS_INC_H
#define OS_INC_H

#include "os_files.h"
//#include "os_network.h"
#include "os_time.h"

#include "os_key.h"
#include "os_event.h"
#include "os_state.h"

#if OS_LINUX
#include "linux/linux_inc.h"
#elif OS_WINDOWS
#include "win32/win32_inc.h"
#else
# error Provide Platform Implementations
#endif

#include "os_semaphore.h"
#include "os_mutex.h"

#endif //OS_INC_H
