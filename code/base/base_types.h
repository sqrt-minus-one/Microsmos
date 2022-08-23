/* date = June 20th 2022 0:14 pm */

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

////////////////////////////////
//~ NOTE(fakhri): "Keywords"

#define global         static
#define internal       static
#define local_persist  static

#if LANG_CPP
# define ZERO_STRUCT {}
#else
# define ZERO_STRUCT {0}
#endif

#if LANG_CPP
#if OS_WINDOWS
#define exported extern "C" __declspec(dllexport)
#else
#define exported extern "C"
#endif
#else
#if OS_WINDOWS
#define exported __declspec(dllexport)
#else
#define exported
#endif
#endif

#if LANG_CPP
#if OS_WINDOWS
#define imported extern "C" __declspec(dllimport)
#else
#define imported extern "C"
#endif
#else
#if OS_WINDOWS
#define imported __declspec(dllimport)
#else
#define imported
#endif
#endif

#if COMPILER_CL
#define per_thread __declspec(thread)
#elif COMPILER_CLANG
#define per_thread __thread
#elif COMPILER_GCC
#define per_thread __thread
#endif

#if COMPILER_CL && COMPILER_CL_YEAR < 2015
# define inline_internal static
#else
# define inline_internal inline static
#endif

#if COMPILER_CL && COMPILER_CL_YEAR < 2015
# define this_function_name "unknown"
#else
# define this_function_name __func__
#endif

#define fallthrough

#if OS_WINDOWS
#pragma section(".roglob", read)
#define read_only __declspec(allocate(".roglob"))
#elif COMPILER_GCC
# define read_only __attribute__((section(".rodata#")))
#else
// TODO(fakhri): figure out if this benefit is possible on other platforms
#define read_only
#endif

////////////////////////////////
//~ NOTE(fakhri): Simple Helper Macros
#define Statement(S) do { S } while(0)

#define MemoryCopy memcpy
#define MemoryMove memmove
#define MemorySet  memset

#define MemoryCopyStruct(d,s) Statement(MemoryCopy((d),(s),sizeof(*(d)));)
#define MemoryCopyArray(d,s) Statement(Assert(sizeof(d)==sizeof(s)); MemoryCopy((d),(s),sizeof(s));)

#define MemoryZero(p,s) MemorySet((p), 0, (s))
#define MemoryZeroStruct(p) MemoryZero((p), sizeof(*(p)))
#define MemoryZeroArray(a) MemoryZero((a), sizeof(a))

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define IntFromPtr(p) (u64)(((u8*)p) - 0)
#define PtrFromInt(i) (void*)(((u8*)0) + i)
#define Member(S,m) ((S*)0)->m
#define OffsetOf(S,m) IntFromPtr(&Member(S,m))
#define CastFromMember(S,m,p) (S*)(((u8*)p) - OffsetOf(S,m))
#define MemberFromOffset(ptr, off, type) *(type *)((u8 *)(ptr) + off)
#define UnusedVariable(name) (void)name

#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((u64)n) << 30)
#define Terabytes(n)  (((u64)n) << 40)

#define Thousand(x) ((x)*1000)
#define Million(x) ((x)*1000000)
#define Billion(x) ((x)*1000000000LL)

#define Meter(x) (x)
#define CentiMeter(x) ((x) / 100.0f)
#define MiliMeter(x) ((x) / 1000.0f)

#define HasFlag(fl,fi) ((fl)&(fi))
#define SetFlag(fl,fi) ((fl)|=(fi))
#define RemFlag(fl,fi) ((fl)&=~(fi))
#define ToggleFlag(fl,fi) ((fl)^=(fi))

#define Swap(T,a,b) Statement(T t__ = a; a = b; b = t__;)

#define AlignForward(n,a)   (((n) + (a) - 1) & ~((a)-1))
////////////////////////////////
//~ NOTE(fakhri): Linked-List Macros

#define CheckNull(p) ((p)==0)
#define SetNull(p) ((p)=0)

#define QueuePush_NZ(f,l,n,Next,zchk,zset) (zchk(f)?\
(((f)=(l)=(n)), zset((n)->Next)):\
((l)->Next=(n),(l)=(n),zset((n)->Next)))
#define QueuePushFront_NZ(f,l,n,Next,zchk,zset) (zchk(f) ? (((f) = (l) = (n)), zset((n)->Next)) :\
((n)->Next = (f)), ((f) = (n)))
#define QueuePop_NZ(f,l,next,zset) ((f)==(l)?\
(zset(f),zset(l)):\
((f)=(f)->next))

#define StackPush_N(f,n,Next) ((n)->Next=(f),(f)=(n))
#define StackPop_NZ(f,Next,zchk) (zchk(f)?0:((f)=(f)->Next))

#define DLLInsert_NPZ(f,l,p,n,Next,Prev,zchk,zset) \
(zchk(f) ? (((f) = (l) = (n)), zset((n)->Next), zset((n)->Prev)) :\
zchk(p) ? (zset((n)->Prev), (n)->Next = (f), (zchk(f) ? (0) : ((f)->Prev = (n))), (f) = (n)) :\
((zchk((p)->Next) ? (0) : (((p)->Next->Prev) = (n))), (n)->Next = (p)->Next, (n)->Prev = (p), (p)->Next = (n),\
((p) == (l) ? (l) = (n) : (0))))
#define DLLPushBack_NPZ(f,l,n,Next,Prev,zchk,zset) DLLInsert_NPZ(f,l,l,n,Next,Prev,zchk,zset)
#define DLLRemove_NPZ(f,l,n,Next,Prev,zchk,zset) (((f)==(n))?\
((f)=(f)->Next, (zchk(f) ? (zset(l)) : zset((f)->Prev))):\
((l)==(n))?\
((l)=(l)->Prev, (zchk(l) ? (zset(f)) : zset((l)->Next))):\
((zchk((n)->Next) ? (0) : ((n)->Next->Prev=(n)->Prev)),\
(zchk((n)->Prev) ? (0) : ((n)->Prev->Next=(n)->Next))))


#define QueuePush(f,l,n)         QueuePush_NZ(f,l,n,Next,CheckNull,SetNull)
#define QueuePushFront(f,l,n)    QueuePushFront_NZ(f,l,n,Next,CheckNull,SetNull)
#define QueuePop(f,l)            QueuePop_NZ(f,l,Next,SetNull)
#define StackPush(f,n)           StackPush_N(f,n,Next)
#define StackPop(f)              StackPop_NZ(f,Next,CheckNull)
#define DLLPushBack(f,l,n)       DLLPushBack_NPZ(f,l,n,Next,Prev,CheckNull,SetNull)
#define DLLPushFront(f,l,n)      DLLPushBack_NPZ(l,f,n,Prev,Next,CheckNull,SetNull)
#define DLLInsert(f,l,p,n)       DLLInsert_NPZ(f,l,p,n,Next,Prev,CheckNull,SetNull)
#define DLLRemove(f,l,n)         DLLRemove_NPZ(f,l,n,Next,Prev,CheckNull,SetNull)

////////////////////////////////
//~ NOTE(fakhri): Basic Types

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8       b8;
typedef i16      b16;
typedef i32      b32;
typedef i64      b64;
typedef float    r32;
typedef double   r64;
typedef size_t   usize;
typedef void void_function(void);

#if LANG_C
enum
{
    false,
    true,
};
#endif
////////////////////////////////
//~ NOTE(fakhri): Limits

read_only global u8 u8Max = 0xFF;
read_only global u8 u8Min = 0;

read_only global u16 u16Max = 0xFFFF;
read_only global u16 u16Min = 0;

read_only global u32 u32Max = 0xFFFFFFFF;
read_only global u32 u32Min = 0;

read_only global u64 u64Max = 0xFFFFFFFFFFFFFFFF;
read_only global u64 u64Min = 0;

read_only global i8 i8Max = 0x7F;
read_only global i8 i8Min = -1 - 0x7F;

read_only global i16 i16Max = 0x7FFF;
read_only global i16 i16Min = -1 - 0x7FFF;

read_only global i32 i32Max = 0x7FFFFFFF;
read_only global i32 i32Min = -1 - 0x7FFFFFFF;

read_only global i64 i64Max = 0x7FFFFFFFFFFFFFFF;
read_only global i64 i64Min = -1 - 0x7FFFFFFFFFFFFFFF;

read_only global u32 Signr32 = 0x80000000;
read_only global u32 Exponentr32 = 0x7F800000;
read_only global u32 Mantissar32 = 0x7FFFFF;

////////////////////////////////
//~ NOTE(fakhri): Constants

read_only global u8 Base64Encoding[64] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z','-', '_'
};

read_only global u64 Bitmask[] =
{
    0x0,
    0x1,
    0x3,
    0x7,
    0xF,
    0x1F,
    0x3F,
    0x7F,
    0xFF,
    0x1FF,
    0x3FF,
    0x7FF,
    0xFFF,
    0x1FFF,
    0x3FFF,
    0x7FFF,
    0xFFFF,
    0x1FFFF,
    0x3FFFF,
    0x7FFFF,
    0xFFFFF,
    0x1FFFFF,
    0x3FFFFF,
    0x7FFFFF,
    0xFFFFFF,
    0x1FFFFFF,
    0x3FFFFFF,
    0x7FFFFFF,
    0xFFFFFFF,
    0x1FFFFFFF,
    0x3FFFFFFF,
    0x7FFFFFFF,
    0xFFFFFFFF,
    0x1FFFFFFFF,
    0x3FFFFFFFF,
    0x7FFFFFFFF,
    0xFFFFFFFFF,
    0x1FFFFFFFFF,
    0x3FFFFFFFFF,
    0x7FFFFFFFFF,
    0xFFFFFFFFFF,
    0x1FFFFFFFFFF,
    0x3FFFFFFFFFF,
    0x7FFFFFFFFFF,
    0xFFFFFFFFFFF,
    0x1FFFFFFFFFFF,
    0x3FFFFFFFFFFF,
    0x7FFFFFFFFFFF,
    0xFFFFFFFFFFFF,
    0x1FFFFFFFFFFFF,
    0x3FFFFFFFFFFFF,
    0x7FFFFFFFFFFFF,
    0xFFFFFFFFFFFFF,
    0x1FFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFF,
    0x7FFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0x1FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x7FFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0x1FFFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFFF,
    0x7FFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFFF,
};

read_only global u32 Bit1  = 1 << 0;
read_only global u32 Bit2  = 1 << 1;
read_only global u32 Bit3  = 1 << 2;
read_only global u32 Bit4  = 1 << 3;
read_only global u32 Bit5  = 1 << 4;
read_only global u32 Bit6  = 1 << 5;
read_only global u32 Bit7  = 1 << 6;
read_only global u32 Bit8  = 1 << 7;
read_only global u32 Bit9  = 1 << 8;
read_only global u32 Bit10 = 1 << 9;
read_only global u32 Bit11 = 1 << 10;
read_only global u32 Bit12 = 1 << 11;
read_only global u32 Bit13 = 1 << 12;
read_only global u32 Bit14 = 1 << 13;
read_only global u32 Bit15 = 1 << 14;
read_only global u32 Bit16 = 1 << 15;
read_only global u32 Bit17 = 1 << 16;
read_only global u32 Bit18 = 1 << 17;
read_only global u32 Bit19 = 1 << 18;
read_only global u32 Bit20 = 1 << 19;
read_only global u32 Bit21 = 1 << 20;
read_only global u32 Bit22 = 1 << 21;
read_only global u32 Bit23 = 1 << 22;
read_only global u32 Bit24 = 1 << 23;
read_only global u32 Bit25 = 1 << 24;
read_only global u32 Bit26 = 1 << 25;
read_only global u32 Bit27 = 1 << 26;
read_only global u32 Bit28 = 1 << 27;
read_only global u32 Bit29 = 1 << 28;
read_only global u32 Bit30 = 1 << 29;
read_only global u32 Bit31 = 1 << 30;
read_only global u32 Bit32 = 1u << 31;

read_only global r32 r32Max = 3.4028234664e+38f;
read_only global r32 r32Min = -3.4028234664e+38f;
read_only global r32 r32SmallestPositive = 1.1754943508e-38f;
read_only global r32 r32Epsilon = 5.96046448e-8f;
read_only global r32 r32Tau = 6.28318530718f;
read_only global r32 r32Pi = 3.14159265359f;

////////////////////////////////
//~ NOTE(fakhri): Base Enums

typedef enum Side
{
    Side_Invalid = -1,
    Side_Min,
    Side_Max,
    Side_COUNT
}
Side;

typedef enum Axis2
{
    Axis2_X,
    Axis2_Y,
    Axis2_COUNT
}
Axis2;

typedef enum Axis3
{
    Axis3_X,
    Axis3_Y,
    Axis3_Z,
    Axis3_COUNT
}
Axis3;

typedef enum Axis4
{
    Axis4_X,
    Axis4_Y,
    Axis4_Z,
    Axis4_COUNT
}
Axis4;

typedef enum DimensionAxis
{
    Dimension_X,
    Dimension_Y,
    Dimension_Z,
    Dimension_W,
}
DimensionAxis;

typedef enum comparison
{
    comparison_EqualTo,
    comparison_NotEqualTo,
    comparison_LessThan,
    comparison_LessThanOrEqualTo,
    comparison_GreaterThan,
    comparison_GreaterThanOrEqualTo,
}
comparison;

////////////////////////////////
//~ NOTE(fakhri): Toggles
#define ENABLE_ASSERTS 0
#define ENABLE_LOGS    1

////////////////////////////////
//~ NOTE(fakhri): Assertions

#undef Assert

#if ENABLE_ASSERTS
#define AssertBreak (*(volatile int *)0 = 0)
#define Assert(b) Statement(if (!(b)) { Log("Assertion Assert(%s) faild", #b); AssertBreak; })
#else
#define AssertBreak
#define Assert(b) (b)
#endif
#define StaticAssert(c,label) u8 static_Assert_##label[(c)?(1):(-1)]
#define NotImplemented Assert(!"Not Implemented")
#define InvalidPath Assert(!"Invalid Path")

////////////////////////////////
//~ NOTE(fakhri): Logging
#if ENABLE_LOGS
#define Log(...)         _DebugLog(0,           __FILE__, __LINE__, __VA_ARGS__)
#define LogWarning(...)  _DebugLog(Log_Warning, __FILE__, __LINE__, __VA_ARGS__)
#define LogError(...)    _DebugLog(Log_Error,   __FILE__, __LINE__, __VA_ARGS__)
#else
#define Log(...)
#define LogWarning(...)
#define LogError(...)
#endif

#define Log_Warning (1<<0)
#define Log_Error   (1<<1)

internal void _DebugLog(i32 flags, char *file, int line, char *format, ...);

////////////////////////////////
//~ NOTE(fakhri): Bit Patterns

inline_internal r32
AbsoluteValuer32(r32 F)
{
    union { u32 U; r32 F; } X;
    X.F = F;
    X.U = X.U & ~Signr32;
    return(X.F);
}

////////////////////////////////
//~ NOTE(fakhri): Comparisons

internal b32 Compare_u64(u64 A, u64 B, comparison Comparison);

// NOTE(fakhri): OS stuff
#if OS_LINUX
#include <semaphore.h>
#include <pthread.h>
typedef pthread_mutex_t mutex_handle;
typedef sem_t semaphore_handle;
typedef int socket_handle;
#define InvalidSocket (-1)
# define SYS_THREAD_SIG(ThreadName) void *ThreadName(void *Input)
#elif OS_WINDOWS
#include <windows.h>
#include <windowsx.h>
typedef HANDLE mutex_handle;
typedef HANDLE semaphore_handle;
typedef SOCKET socket_handle;
#define InvalidSocket (INVALID_SOCKET)
# define SYS_THREAD_SIG(ThreadName) DWORD WINAPI ThreadName(_In_ LPVOID Input)
#else
#error add OS specific types
#endif


#if COMPILER_CL
# define AtomicCompareAndExchange(Address, ExpectedValue, ExchangeValue) \
(InterlockedCompareExchange(Address, ExchangeValue, ExpectedValue) == (ExpectedValue))
# define AtomicCompareAndExchange64(Address, ExpectedValue, ExchangeValue) \
(InterlockedCompareExchange64((volatile LONG64 *)Address, ExchangeValue, ExpectedValue) == (LONG64)(ExpectedValue))
# define CompletePreviousWritesBeforeFutureWrites() MemoryBarrier(); __faststorefence()
# define AtomicIncrement(Address) InterlockedIncrement(Address)
# define AtomicIncrement64(Address) InterlockedIncrement64((volatile LONG64 *)Address)
# define AtomicDecrement64(Address) InterlockedDecrement64((volatile LONG64 *)Address)
#elif COMPILER_GCC
# define AtomicCompareAndExchange(Address, ExpectedValue, ExchangeValue) __atomic_compare_exchange_n (Address, &ExpectedValue, ExchangeValue, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
# define CompletePreviousWritesBeforeFutureWrites() __atomic_thread_fence (__ATOMIC_SEQ_CST)
# define AtomicIncrement(Address) __atomic_fetch_add (Address, 1, __ATOMIC_SEQ_CST)
# define AtomicDecrement(Address) __atomic_fetch_sub (Address, 1, __ATOMIC_SEQ_CST)
#endif

#endif //BASE_TYPES_H
