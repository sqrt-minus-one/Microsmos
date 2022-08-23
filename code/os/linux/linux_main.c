
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/resource.h>

#include "base/base_inc.h"
#include "os/os_inc.h"

#include "base/base_inc.c"
#include "os/os_inc.c"

int main()
{
    signal(SIGPIPE, SIG_IGN);
    
    MainThreadContext = MakeThreadContext();
    SetThreadContext(&MainThreadContext);
    m_arena *Arena = M_ArenaAllocDefault();
    // NOTE(fakhri): launch worker threads
    for (u32 ThreadIndex = 0;
         ThreadIndex < WorkerThreadsCount;
         ++ThreadIndex)
    {
        struct worker_info *WorkerInfo = PushStructZero(Arena, struct worker_info);
        
        WorkerInfo->WorkQueue = &GlobalWorkQueue;
        WorkerInfo->WorkerID = ThreadIndex;
        
        pthread_t ThreadID;
        pthread_create(&ThreadID, 0, WorkerThreadMain, WorkerInfo);
        pthread_detach(ThreadID);
    }
    
    return 0;
}


