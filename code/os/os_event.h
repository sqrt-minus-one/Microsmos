/* date = August 19th 2022 0:47 pm */

#ifndef OS_EVENT_H
#define OS_EVENT_H

typedef enum os_event_kind
{
    OS_EventKind_Null,
    OS_EventKind_Press,
    OS_EventKind_Release,
    OS_EventKind_Text,
    OS_EventKind_MouseScroll,
    OS_EventKind_COUNT
} os_event_kind;

typedef u32 os_modiferes;
enum
{
    OS_Modifier_Ctrl  = (1<<0),
    OS_Modifier_Shift = (1<<1),
    OS_Modifier_Alt   = (1<<2),
};

typedef struct os_event os_event;
struct os_event
{
    os_event *Next;
    os_event *Prev;
    
    os_event_kind Kind;
    os_modiferes Modifiers;
    os_key Key;
    u32 Character;
    v2r32 Position;
    v2r32 Scroll;
    string8 Path;
};

typedef struct os_event_list os_event_list;
struct os_event_list
{
    os_event *First;
    os_event *Last;
    u64 Count;
    
    os_event *FreeEvents;
};

internal os_event *OS_MakeEvent(m_arena *Arena, os_event_list *List);
internal void OS_EatEvent(os_event_list *Events, os_event *Event);
internal void OS_EatAllEvents(os_event_list *EventList);

#endif //OS_EVENT_H
