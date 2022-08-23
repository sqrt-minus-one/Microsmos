
internal hashtable
MakeHashtableOfSize(m_arena *Arena, u32 BacketCount)
{
    hashtable Result = ZERO_STRUCT;
    Result.Backets     = PushArrayZero(Arena, hashtable_backet, BacketCount);
    Result.BacketCount = BacketCount;
    return Result;
}

internal hashtable
MakeHashtable(m_arena *Arena)
{
    hashtable Result = MakeHashtableOfSize(Arena, 2053);
    return Result;
}

internal hashtable_key
MakeHashtableKey(string8 String)
{
    hashtable_key Result;
    Result.Hash         = HashString(String);
    Result.Content.str  = String.str;
    Result.Content.size = String.size;
    return Result;
}

internal u32
HashString(string8 String)
{
    u32 HashValue = 0;
    for (u32 Index = 0;
         Index < String.size;
         ++Index)
    {
        u8 Ch = String.str[Index];
        HashValue += (HashValue * 33) ^ Ch;
    }
    return HashValue;
}

internal hashtable_slot *
Hashtable_Insert(m_arena *Arena, hashtable *Hashtable, hashtable_key Key, void *Value)
{
    hashtable_slot *Result = 0;
    if (Hashtable->BacketCount)
    {
        u64 Index = Key.Hash % Hashtable->BacketCount;
        hashtable_backet *Backet = Hashtable->Backets + Index;
        
        if (Hashtable->FreeSlots.First)
        {
            Result = Hashtable->FreeSlots.First;
            QueuePop(Hashtable->FreeSlots.First, Hashtable->FreeSlots.Last);
        }
        
        if (!Result)
        {
            Result = PushStructZero(Arena, hashtable_slot);
        }
        
        DLLPushBack(Backet->First, Backet->Last, Result);
        Result->Key   = Key;
        Result->Value = Value;
        ++Hashtable->ItemsCount;
    }
    return Result;
}

internal hashtable_slot *
Hashtable_Find(hashtable *Hashtable, hashtable_key Key)
{
    hashtable_slot *Result = 0;
    Assert(Hashtable->Backets);
    if (Hashtable && Hashtable->Backets)
    {
        u64 Index = Key.Hash % Hashtable->BacketCount;
        hashtable_backet *Backet = Hashtable->Backets + Index;
        for (hashtable_slot *Slot = Backet->First;
             Slot;
             Slot = Slot->Next)
        {
            if (Slot->Key.Hash == Key.Hash && Str8Match(Slot->Key.Content, Key.Content, 0))
            {
                Result = Slot;
                break;
            }
        }
    }
    return Result;
}


internal hashtable_slot *
Hashtable_InsertOverwrite(m_arena *Arena, hashtable *Hashtable, hashtable_key Key, void *Value)
{
    hashtable_slot *Result = Hashtable_Find(Hashtable, Key);
    if (Result)
    {
        Result->Value = Value;
    }
    else
    {
        Result = Hashtable_Insert(Arena, Hashtable, Key, Value);
    }
    return Result;
}

internal void
Hashtable_RemoveSlot(hashtable *Hashtable, hashtable_slot *Slot)
{
    u32 BacketIndex = Slot->Key.Hash % Hashtable->BacketCount;
    hashtable_backet *Backet = Hashtable->Backets + BacketIndex;
    DLLRemove(Backet->First, Backet->Last, Slot);
    QueuePush(Hashtable->FreeSlots.First, Hashtable->FreeSlots.Last, Slot);
    --Hashtable->ItemsCount;
}

internal b32
Hashtable_Remove(hashtable *Hashtable, hashtable_key Key)
{
    b32 Result = false;
    hashtable_slot *Slot = Hashtable_Find(Hashtable, Key);
    if (Slot)
    {
        Result = true;
        Hashtable_RemoveSlot(Hashtable, Slot);
    }
    return Result;
}

internal hashtable_iterator
Hashtable_MakeIterator(hashtable *Hashtable)
{
    hashtable_iterator Result;
    Result.Hashtable = Hashtable;
    Result.NextBacketIndex = 0;
    Result.CurrentSlot = 0;
    HashtableIter_Advance(&Result);
    return Result;
}

internal void
HashtableIter_Advance(hashtable_iterator *Iter)
{
    if (Iter->CurrentSlot)
    {
        Iter->CurrentSlot = Iter->CurrentSlot->Next;
    }
    
    if (!Iter->CurrentSlot)
    {
        for (;Iter->NextBacketIndex < Iter->Hashtable->BacketCount;)
        {
            hashtable_backet *Backet = Iter->Hashtable->Backets + Iter->NextBacketIndex++;
            if (Backet->First)
            {
                Iter->CurrentSlot = Backet->First;
                break;
            }
        }
    }
}

internal b32
HashtableIter_Done(hashtable_iterator *Iter)
{
    b32 Result = (Iter->CurrentSlot == 0);
    return Result;
}

internal hashtable_slot *
HashtableIter_GetSlot(hashtable_iterator *Iter)
{
    hashtable_slot *Result = Iter->CurrentSlot;
    return Result;
}

