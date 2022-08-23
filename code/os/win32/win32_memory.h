/* date = July 22nd 2022 9:01 pm */

#ifndef WIN32_MEMORY_H
#define WIN32_MEMORY_H


#define M_IMPL_Reserve  W32_Reserve
#define M_IMPL_Release  W32_Release
#define M_IMPL_Commit   W32_Commit
#define M_IMPL_Decommit W32_Decommit

internal void *W32_Reserve(u64 Size);
internal void  W32_Release(void *Memory, u64 Size);
internal b32   W32_Commit(void *Memory, u64 Size);
internal void  W32_Decommit(void *Memory, u64 Size);

#endif //WIN32_MEMORY_H
