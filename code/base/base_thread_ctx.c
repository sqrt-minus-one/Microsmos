per_thread thread_ctx *TL_ThreadCtx = 0;

internal thread_ctx
MakeThreadContext(void)
{
    thread_ctx ThreadCtx = {0};
    for(u64 ArenaIndex = 0; ArenaIndex < ArrayCount(ThreadCtx.Arenas); ArenaIndex += 1)
    {
        ThreadCtx.Arenas[ArenaIndex] = M_ArenaAlloc(Gigabytes(8));
    }
    return ThreadCtx;
}

internal void
SetThreadCtx(thread_ctx *tctx)
{
    TL_ThreadCtx = tctx;
}

internal thread_ctx *
GetThreadCtx(void)
{
    return TL_ThreadCtx;
}

internal void
RegisterThreadFileAndLine_(char *File, int line)
{
    thread_ctx *tctx = GetThreadCtx();
    tctx->FileName = File;
    tctx->LineNumber = line;
}

internal m_temp
GetScratch(m_arena **conflicts, u64 conflict_count)
{
    m_temp scratch = {0};
    thread_ctx *tctx = GetThreadCtx();
    for(u64 tctx_idx = 0; tctx_idx < ArrayCount(tctx->Arenas); tctx_idx += 1)
    {
        b32 is_conflicting = 0;
        for(m_arena **conflict = conflicts; conflict < conflicts+conflict_count; conflict += 1)
        {
            if(*conflict == tctx->Arenas[tctx_idx])
            {
                is_conflicting = 1;
                break;
            }
        }
        if(is_conflicting == 0)
        {
            scratch.Arena = tctx->Arenas[tctx_idx];
            scratch.Pos = scratch.Arena->Pos;
            break;
        }
    }
    return scratch;
}
