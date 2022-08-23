
internal os_event *
OS_MakeEvent(m_arena *Arena, os_event_list *EventList)
{
    os_event *Result = 0;
    if (EventList->FreeEvents)
    {
        Result = EventList->FreeEvents;
        StackPop(EventList->FreeEvents);
    }
    
    if (!Result)
    {
        Result = PushStruct(Arena, os_event);
    }
    
    if (Result)
    {
        MemoryZeroStruct(Result);
    }
    
    return Result;
}

internal void
OS_EventList_PushEvent(os_event_list *EventList, os_event *Event)
{
    DLLPushBack(EventList->First, EventList->Last, Event);
    EventList->Count += 1;
}

internal void
OS_EatEvent(os_event_list *EventList, os_event *Event)
{
    DLLRemove(EventList->First, EventList->Last, Event);
    EventList->Count -= 1;
    StackPush(EventList->FreeEvents, Event);
}

internal void
OS_EatAllEvents(os_event_list *EventList)
{
    if (EventList->Last)
    {
        StackPush(EventList->FreeEvents, EventList->Last);
        EventList->FreeEvents = EventList->First;
        EventList->First = EventList->Last = 0;
        EventList->Count = 0;
    }
}
