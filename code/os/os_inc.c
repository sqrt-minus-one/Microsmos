
#include "os_files.c"
//#include "os_network.c"
#include "os_semaphore.c"
#include "os_mutex.c"
#include "os_time.c"

#include "os_key.c"
#include "os_event.c"
#include "os_state.c"

#if OS_LINUX
#include "linux/linux_inc.c"
#elif OS_WINDOWS
#include "win32/win32_inc.c"
#else
# error Provide Platform Implementations
#endif

