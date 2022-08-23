/* date = June 20th 2022 0:45 pm */

#ifndef BASE_THREAD_CTX_H
#define BASE_THREAD_CTX_H

typedef struct thread_ctx thread_ctx;
struct thread_ctx
{
    m_arena *Arenas[2];
    char *FileName;
    u64 LineNumber;
};

internal thread_ctx  MakeThreadCtx(void);
internal void        SetThreadCtx(thread_ctx *tctx);
internal thread_ctx *GetThreadCtx(void);

internal void RegisterThreadFileAndLine_(char *file, int line);
#define RegisterFileAndLine() RegisterThreadFileAndLine_(__FILE__, __LINE__)

internal m_temp GetScratch(m_arena **conflicts, u64 conflict_count);
#define ReleaseScratch(temp) M_EndTemp(temp)

#endif //BASE_THREAD_CTX_H
