/* date = July 27th 2022 2:25 pm */

#ifndef BASE_HASHTABLE_H
#define BASE_HASHTABLE_H

typedef struct hashtable_key hashtable_key;
struct hashtable_key
{
    u64 Hash;
    string8 Content;
};

typedef struct hashtable_slot hashtable_slot;
struct hashtable_slot
{
    struct hashtable_slot *Next;
    struct hashtable_slot *Prev;
    hashtable_key Key;
    void *Value;
};


typedef struct hashtable_backet hashtable_backet;
struct hashtable_backet
{
    hashtable_slot *First;
    hashtable_slot *Last;
};

typedef struct hashtable hashtable;
struct hashtable
{
    hashtable_backet *Backets;
    u32 BacketCount;
    u64 ItemsCount;
    hashtable_backet FreeSlots;
};

typedef struct hashtable_iterator hashtable_iterator;
struct hashtable_iterator
{
    hashtable *Hashtable;
    u64 NextBacketIndex;
    hashtable_slot *CurrentSlot;
};

#define Hashtable_InsertTypedValue(Arena, HashtableAddr, Key, value_type, SourceValueAddr)\
{\
m_temp TempArena = M_BeginTemp(Arena);\
value_type *TypedValue = PushStruct(Arena, value_type);\
MemoryCopyStruct(TypedValue, SourceValueAddr);\
hashtable_slot *_Result = Hashtable_Insert(Arena, HashtableAddr, Key, TypedValue);\
if (!_Result)\
{\
M_EndTemp(TempArena);\
}\
}


internal u32 HashString(string8 String);
internal hashtable_key MakeHashtableKey(string8 String);
internal hashtable_slot *Hashtable_Insert(m_arena *Arena, hashtable *Hashtable, hashtable_key Key, void *Value);
internal hashtable_slot *Hashtable_Find(hashtable *Hashtable, hashtable_key Key);
internal hashtable_slot *Hashtable_InsertOverwrite(m_arena *Arena, hashtable *Hashtable, hashtable_key Key, void *Value);
internal void Hashtable_RemoveSlot(hashtable *Hashtable, hashtable_slot *Slot);
internal b32 Hashtable_Remove(hashtable *Hashtable, hashtable_key Key);
internal hashtable MakeHashtableOfSize(m_arena *Arena, u32 BacketCount);
internal hashtable MakeHashtable(m_arena *Arena);

internal hashtable_iterator Hashtable_MakeIterator(hashtable *Hashtable);
internal void HashtableIter_Advance(hashtable_iterator *Iter);
internal b32 HashtableIter_Done(hashtable_iterator *Iter);
internal hashtable_slot *HashtableIter_GetSlot(hashtable_iterator *Iter);


inline_internal void
Hashtable_InsertString(m_arena *Arena, hashtable *Hashtable, string8 Key, string8 *String)
{
    Hashtable_InsertTypedValue(Arena, Hashtable, MakeHashtableKey(Key), string8, String);
}

inline_internal string8 *
Hashtable_FindString(hashtable *Hashtable, string8 Key)
{
    hashtable_slot *Slot = Hashtable_Find(Hashtable, MakeHashtableKey(Key));
    string8 *Result = (string8*)(Slot? Slot->Value:0);
    return Result;
}

#endif //BASE_HASHTABLE_H
