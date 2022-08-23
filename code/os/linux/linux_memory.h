/* date = June 20th 2022 6:17 pm */

#ifndef LINUX_MEMORY_H
#define LINUX_MEMORY_H

#define M_IMPL_Reserve  Linux_Reserve
#define M_IMPL_Release  Linux_Release
#define M_IMPL_Commit   Linux_Commit
#define M_IMPL_Decommit Linux_Decommit

internal void *Linux_Reserve(u64 Size);
internal void  Linux_Release(void *Memory, u64 Size);
internal b32  Linux_Commit(void *Memory, u64 Size);
internal void  Linux_Decommit(void *Memory, u64 Size);

#endif //LINUX_MEMORY_H
