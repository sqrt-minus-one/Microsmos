
////////////////////////////////
//~ NOTE(fakhri): Char Functions

internal b32
CharIsAlpha(u8 c)
{
    return CharIsAlphaUpper(c) || CharIsAlphaLower(c);
}

internal b32
CharIsAlphaUpper(u8 c)
{
    return c >= 'A' && c <= 'Z';
}

internal b32
CharIsAlphaLower(u8 c)
{
    return c >= 'a' && c <= 'z';
}

internal b32
CharIsDigit(u8 c)
{
    return (c >= '0' && c <= '9');
}

internal b32
CharIsSymbol(u8 c)
{
    return (c == '~' || c == '!'  || c == '$' || c == '%' || c == '^' ||
            c == '&' || c == '*'  || c == '-' || c == '=' || c == '+' ||
            c == '<' || c == '.'  || c == '>' || c == '/' || c == '?' ||
            c == '|' || c == '\\' || c == '{' || c == '}' || c == '(' ||
            c == ')' || c == '\\' || c == '[' || c == ']' || c == '#' ||
            c == ',' || c == ';'  || c == ':' || c == '@');
}

internal b32
CharIsSpace(u8 c)
{
    return c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v';
}

internal u8
CharToUpper(u8 c)
{
    return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

internal u8
CharToLower(u8 c)
{
    return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}

internal u8
CharToForwardSlash(u8 c)
{
    return (c == '\\' ? '/' : c);
}

////////////////////////////////
//~ NOTE(fakhri): String Functions

//- NOTE(fakhri): Helpers

internal u64
CalculateCStringLength(char *cstr)
{
    u64 length = 0;
    for(;cstr[length]; length += 1);
    return length;
}

//- NOTE(fakhri): Basic Constructors

internal string8
Str8(u8 *str, u64 size)
{
    string8 string;
    string.str = str;
    string.size = size;
    return string;
}

internal string8
Str8Range(u8 *First, u8 *one_past_Last)
{
    string8 string;
    string.str = First;
    string.size = (u64)(one_past_Last - First);
    return string;
}

internal string16
Str16(u16 *str, u64 size)
{
    string16 result;
    result.str = str;
    result.size = size;
    return result;
}

internal string16
Str16C(u16 *ptr)
{
    u16 *p = ptr;
    for (;*p; p += 1);
    string16 result = Str16(ptr, p - ptr);
    return(result);
}

//- NOTE(fakhri): Substrings

internal string8
Substr8(string8 str, u64 min, u64 max)
{
    if(max > str.size)
    {
        max = str.size;
    }
    if(min > str.size)
    {
        min = str.size;
    }
    if(min > max)
    {
        u64 swap = min;
        min = max;
        max = swap;
    }
    str.size = max - min;
    str.str += min;
    return str;
}

internal string8
Str8SkipFirst(string8 str, u64 min)
{
    return Substr8(str, min, str.size);
}

internal string8
Str8ChopLast(string8 str, u64 nmax)
{
    return Substr8(str, 0, str.size-nmax);
}

internal string8
Prefix8(string8 str, u64 size)
{
    return Substr8(str, 0, size);
}

internal string8
Suffix8(string8 str, u64 size)
{
    return Substr8(str, str.size-size, str.size);
}

//- NOTE(fakhri): Matching

internal b32
Str8Match(string8 a, string8 b, match_flags flags)
{
    b32 result = 0;
    if(a.size == b.size || flags & MatchFlag_RightSideSloppy)
    {
        result = 1;
        for(u64 i = 0; i < a.size; i += 1)
        {
            b32 match = (a.str[i] == b.str[i]);
            if(flags & MatchFlag_CaseInsensitive)
            {
                match |= (CharToLower(a.str[i]) == CharToLower(b.str[i]));
            }
            if(flags & MatchFlag_SlashInsensitive)
            {
                match |= (CharToForwardSlash(a.str[i]) == CharToForwardSlash(b.str[i]));
            }
            if(match == 0)
            {
                result = 0;
                break;
            }
        }
    }
    return result;
}

internal u64
FindSubstr8(string8 haystack, string8 needle, u64 start_pos, match_flags flags)
{
    u64 found_idx = haystack.size;
    b32 is_first = true;
    for(u64 i = start_pos; i < haystack.size; i += 1)
    {
        if(i + needle.size <= haystack.size)
        {
            string8 substr = Substr8(haystack, i, i+needle.size);
            if(Str8Match(substr, needle, flags))
            {
                found_idx = i;
                if ((flags & MatchFlag_SkipFirst) && is_first)
                {
                    is_first = false;
                    continue;
                }
                if(!(flags & MatchFlag_FindLast))
                {
                    break;
                }
            }
        }
    }
    return found_idx;
}

internal b32
Str8EndsWith(string8 A, string8 B, match_flags Flags)
{
    b32 Result = 0;
    if (A.size >= B.size)
    {
        for(u32 i = 0;
            i < B.size;
            ++i)
        {
            b32 Match = (A.str[A.size - 1 - i] == B.str[B.size - 1 - i]);
            if(Flags & MatchFlag_CaseInsensitive)
            {
                Match |= (CharToLower(A.str[i]) == CharToLower(B.str[i]));
            }
            if(Flags & MatchFlag_SlashInsensitive)
            {
                Match |= (CharToForwardSlash(A.str[i]) == CharToForwardSlash(B.str[i]));
            }
            if(!Match)
            {
                Result = 0;
                break;
            }
        }
    }
    return Result;
}

//- NOTE(fakhri): Allocation

internal string8
PushStr8Copy(m_arena *arena, string8 string)
{
    string8 res;
    res.size = string.size;
    res.str = PushArray(arena, u8, string.size + 1);
    MemoryCopy(res.str, string.str, string.size);
    res.str[string.size] = 0;
    return res;
}

internal string8
PushStr8FV(m_arena *arena, char *fmt, va_list args)
{
    string8 result = {0};
    va_list args2;
    va_copy(args2, args);
    u64 needed_bytes = stbsp_vsnprintf(0, 0, fmt, args)+1;
    result.str = PushArray(arena, u8, needed_bytes);
    result.size = needed_bytes - 1;
    stbsp_vsnprintf((char*)result.str, (int)needed_bytes, fmt, args2);
    return result;
}

internal string8
PushStr8F(m_arena *arena, char *fmt, ...)
{
    string8 result = {0};
    va_list args;
    va_start(args, fmt);
    result = PushStr8FV(arena, fmt, args);
    va_end(args);
    return result;
}

//- NOTE(fakhri): String Lists

internal void
Str8ListPushNode(string8_list *list, string8_node *n)
{
    QueuePush(list->First, list->Last, n);
    list->NodeCount += 1;
    list->TotalSize += n->String.size;
}

internal string8_node *
Str8ListPopNode(string8_list *list)
{
    string8_node *Result = list->First;
    if (list->First)
    {
        QueuePop(list->First, list->Last);
        list->NodeCount -= 1;
        list->TotalSize -= Result->String.size;
    }
    return Result;
}

internal void
Str8ListPush(m_arena *arena, string8_list *list, string8 str)
{
    string8_node *n = PushArrayZero(arena, string8_node, 1);
    n->String = str;
    Str8ListPushNode(list, n);
}

internal void 
Str8ListPushFV(m_arena *arena, string8_list *list, char *fmt, va_list args)
{
    string8_node *n = PushArrayZero(arena, string8_node, 1);
    n->String = PushStr8FV(arena, fmt, args);
    Str8ListPushNode(list, n);
}

internal void 
Str8ListPushF(m_arena *arena, string8_list *list, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Str8ListPushFV(arena, list, fmt, args);
    va_end(args);
}


internal void
Str8ListConcat(string8_list *list, string8_list *to_push)
{
    if(to_push->First)
    {
        list->NodeCount += to_push->NodeCount;
        list->TotalSize += to_push->TotalSize;
        if(list->Last == 0)
        {
            *list = *to_push;
        }
        else
        {
            list->Last->Next = to_push->First;
            list->Last = to_push->Last;
        }
    }
    MemoryZero(to_push, sizeof(*to_push));
}

internal string8_list
StrSplit8(m_arena *arena, string8 string, int split_count, string8 *splits)
{
    string8_list list = {0};
    
    u64 split_start = 0;
    for(u64 i = 0; i < string.size; i += 1)
    {
        b32 was_split = 0;
        for(int split_idx = 0; split_idx < split_count; split_idx += 1)
        {
            b32 match = 0;
            if(i + splits[split_idx].size <= string.size)
            {
                match = 1;
                for(u64 split_i = 0; split_i < splits[split_idx].size && i + split_i < string.size; split_i += 1)
                {
                    if(splits[split_idx].str[split_i] != string.str[i + split_i])
                    {
                        match = 0;
                        break;
                    }
                }
            }
            if(match)
            {
                string8 split_string = Str8(string.str + split_start, i - split_start);
                Str8ListPush(arena, &list, split_string);
                split_start = i + splits[split_idx].size;
                i += splits[split_idx].size - 1;
                was_split = 1;
                break;
            }
        }
        
        if(was_split == 0 && i == string.size - 1)
        {
            string8 split_string = Str8(string.str + split_start, i+1 - split_start);
            Str8ListPush(arena, &list, split_string);
            break;
        }
    }
    
    return list;
}

internal string8
Str8ListJoin(m_arena *arena, string8_list list, string_join *optional_params)
{
    // NOTE(fakhri): setup join parameters
    string_join join = {0};
    if(optional_params != 0)
    {
        MemoryCopy(&join, optional_params, sizeof(join));
    }
    
    // NOTE(fakhri): calculate size & allocate
    u64 sep_count = 0;
    if(list.NodeCount > 1)
    {
        sep_count = list.NodeCount - 1;
    }
    string8 result = {0};
    result.size = (list.TotalSize + join.Pre.size +
                   sep_count*join.Sep.size + join.Post.size);
    result.str = PushArray(arena, u8, result.size);
    
    // NOTE(fakhri): fill
    u8 *ptr = result.str;
    MemoryCopy(ptr, join.Pre.str, join.Pre.size);
    ptr += join.Pre.size;
    for(string8_node *node = list.First; node; node = node->Next)
    {
        MemoryCopy(ptr, node->String.str, node->String.size);
        ptr += node->String.size;
        if (node != list.Last){
            MemoryCopy(ptr, join.Sep.str, join.Sep.size);
            ptr += join.Sep.size;
        }
    }
    MemoryCopy(ptr, join.Pre.str, join.Pre.size);
    ptr += join.Pre.size;
    
    return result;
}

//- NOTE(fakhri): String Re-Styling

internal string8
Str8Stylize(m_arena *arena, string8 string, identifier_style style, string8 separator)
{
    string8 result = {0};
    
    string8_list words = {0};
    
    b32 break_on_uppercase = 0;
    {
        break_on_uppercase = 1;
        for(u64 i = 0; i < string.size; i += 1)
        {
            if(!CharIsAlpha(string.str[i]) && !CharIsDigit(string.str[i]))
            {
                break_on_uppercase = 0;
                break;
            }
        }
    }
    
    b32 making_word = 0;
    string8 word = {0};
    
    for(u64 i = 0; i < string.size;)
    {
        if(making_word)
        {
            if((break_on_uppercase && CharIsAlphaUpper(string.str[i])) ||
               string.str[i] == '_' || CharIsSpace(string.str[i]) ||
               i == string.size - 1)
            {
                if(i == string.size - 1)
                {
                    word.size += 1;
                }
                making_word = 0;
                Str8ListPush(arena, &words, word);
            }
            else
            {
                word.size += 1;
                i += 1;
            }
        }
        else
        {
            if(CharIsAlpha(string.str[i]))
            {
                making_word = 1;
                word.str = string.str + i;
                word.size = 1;
            }
            i += 1;
        }
    }
    
    result.size = words.TotalSize;
    if(words.NodeCount > 1)
    {
        result.size += separator.size*(words.NodeCount-1);
    }
    result.str = PushArray(arena, u8, result.size);
    
    {
        u64 write_pos = 0;
        for(string8_node *node = words.First; node; node = node->Next)
        {
            
            // NOTE(rjf): Write word string to result.
            {
                MemoryCopy(result.str + write_pos, node->String.str, node->String.size);
                
                // NOTE(rjf): Transform string based on word style.
                switch(style)
                {
                    case identifier_style_UpperCamelCase:
                    {
                        result.str[write_pos] = CharToUpper(result.str[write_pos]);
                        for(u64 i = write_pos+1; i < write_pos + node->String.size; i += 1)
                        {
                            result.str[i] = CharToLower(result.str[i]);
                        }
                    }break;
                    
                    case identifier_style_LowerCamelCase:
                    {
                        result.str[write_pos] = node == words.First ? CharToLower(result.str[write_pos]) : CharToUpper(result.str[write_pos]);
                        for(u64 i = write_pos+1; i < write_pos + node->String.size; i += 1)
                        {
                            result.str[i] = CharToLower(result.str[i]);
                        }
                    }break;
                    
                    case identifier_style_UpperCase:
                    {
                        for(u64 i = write_pos; i < write_pos + node->String.size; i += 1)
                        {
                            result.str[i] = CharToUpper(result.str[i]);
                        }
                    }break;
                    
                    case identifier_style_LowerCase:
                    {
                        for(u64 i = write_pos; i < write_pos + node->String.size; i += 1)
                        {
                            result.str[i] = CharToLower(result.str[i]);
                        }
                    }break;
                    
                    default: break;
                }
                
                write_pos += node->String.size;
            }
            
            if(node->Next)
            {
                MemoryCopy(result.str + write_pos, separator.str, separator.size);
                write_pos += separator.size;
            }
        }
    }
    
    return result;
}

////////////////////////////////
//~ NOTE(fakhri): Unicode Conversions

read_only global u8 utf8_class[32] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

#define bitmask1 0x01
#define bitmask2 0x03
#define bitmask3 0x07
#define bitmask4 0x0F
#define bitmask5 0x1F
#define bitmask6 0x3F
#define bitmask7 0x7F
#define bitmask8 0xFF
#define bitmask9  0x01FF
#define bitmask10 0x03FF

internal decoded_codepoint
DecodeCodepointFromUtf8(u8 *str, u64 max)
{
    decoded_codepoint result = {~((u32)0), 1};
    u8 byte = str[0];
    u8 byte_class = utf8_class[byte >> 3];
    switch (byte_class)
    {
        case 1:
        {
            result.Codepoint = byte;
        }break;
        
        case 2:
        {
            if (2 <= max)
            {
                u8 cont_byte = str[1];
                if (utf8_class[cont_byte >> 3] == 0)
                {
                    result.Codepoint = (byte & bitmask5) << 6;
                    result.Codepoint |=  (cont_byte & bitmask6);
                    result.Advance = 2;
                }
            }
        }break;
        
        case 3:
        {
            if (3 <= max)
            {
                u8 cont_byte[2] = {str[1], str[2]};
                if (utf8_class[cont_byte[0] >> 3] == 0 &&
                    utf8_class[cont_byte[1] >> 3] == 0)
                {
                    result.Codepoint = (byte & bitmask4) << 12;
                    result.Codepoint |= ((cont_byte[0] & bitmask6) << 6);
                    result.Codepoint |=  (cont_byte[1] & bitmask6);
                    result.Advance = 3;
                }
            }
        }break;
        
        case 4:
        {
            if (4 <= max)
            {
                u8 cont_byte[3] = {str[1], str[2], str[3]};
                if (utf8_class[cont_byte[0] >> 3] == 0 &&
                    utf8_class[cont_byte[1] >> 3] == 0 &&
                    utf8_class[cont_byte[2] >> 3] == 0)
                {
                    result.Codepoint = (byte & bitmask3) << 18;
                    result.Codepoint |= ((cont_byte[0] & bitmask6) << 12);
                    result.Codepoint |= ((cont_byte[1] & bitmask6) <<  6);
                    result.Codepoint |=  (cont_byte[2] & bitmask6);
                    result.Advance = 4;
                }
            }
        }break;
    }
    
    return result;
}

internal decoded_codepoint
DecodeCodepointFromUtf16(u16 *out, u64 max)
{
    decoded_codepoint result = {~((u32)0), 1};
    result.Codepoint = out[0];
    result.Advance = 1;
    if (1 < max && 0xD800 <= out[0] && out[0] < 0xDC00 && 0xDC00 <= out[1] && out[1] < 0xE000)
    {
        result.Codepoint = ((out[0] - 0xD800) << 10) | (out[1] - 0xDC00);
        result.Advance = 2;
    }
    return result;
}

internal u32             
Utf8FromCodepoint(u8 *out, u32 codepoint)
{
#define bit8 0x80
    u32 advance = 0;
    if (codepoint <= 0x7F)
    {
        out[0] = (u8)codepoint;
        advance = 1;
    }
    else if (codepoint <= 0x7FF)
    {
        out[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
        out[1] = bit8 | (codepoint & bitmask6);
        advance = 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        out[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
        out[1] = bit8 | ((codepoint >> 6) & bitmask6);
        out[2] = bit8 | ( codepoint       & bitmask6);
        advance = 3;
    }
    else if (codepoint <= 0x10FFFF)
    {
        out[0] = (bitmask4 << 3) | ((codepoint >> 18) & bitmask3);
        out[1] = bit8 | ((codepoint >> 12) & bitmask6);
        out[2] = bit8 | ((codepoint >>  6) & bitmask6);
        out[3] = bit8 | ( codepoint        & bitmask6);
        advance = 4;
    }
    else
    {
        out[0] = '?';
        advance = 1;
    }
    return advance;
}

internal u32             
Utf16FromCodepoint(u16 *out, u32 codepoint)
{
    u32 advance = 1;
    if (codepoint == ~((u32)0))
    {
        out[0] = (u16)'?';
    }
    else if (codepoint < 0x10000)
    {
        out[0] = (u16)codepoint;
    }
    else
    {
        u64 v = codepoint - 0x10000;
        out[0] = (u16)(0xD800 + (v >> 10));
        out[1] = (u16)(0xDC00 + (v & bitmask10));
        advance = 2;
    }
    return advance;
}

internal string8         
Str8From16(m_arena *arena, string16 in)
{
    u64 cap = in.size*3;
    u8 *str = PushArray(arena, u8, cap + 1);
    u16 *ptr = in.str;
    u16 *opl = ptr + in.size;
    u64 size = 0;
    decoded_codepoint consume;
    for (;ptr < opl;)
    {
        consume = DecodeCodepointFromUtf16(ptr, opl - ptr);
        ptr += consume.Advance;
        size += Utf8FromCodepoint(str + size, consume.Codepoint);
    }
    str[size] = 0;
    M_ArenaPop(arena, cap - size); // := ((cap + 1) - (size + 1))
    return Str8(str, size);
}

internal string16        
Str16From8(m_arena *arena, string8 in)
{
    u64 cap = in.size*2;
    u16 *str = PushArray(arena, u16, cap + 1);
    u8 *ptr = in.str;
    u8 *opl = ptr + in.size;
    u64 size = 0;
    decoded_codepoint consume;
    for (;ptr < opl;)
    {
        consume = DecodeCodepointFromUtf8(ptr, opl - ptr);
        ptr += consume.Advance;
        size += Utf16FromCodepoint(str + size, consume.Codepoint);
    }
    str[size] = 0;
    M_ArenaPop(arena, 2*(cap - size)); // := 2*((cap + 1) - (size + 1))
    string16 result = {str, size};
    return result;
}

internal string8         
Str8From32(m_arena *arena, string32 in)
{
    u64 cap = in.size*4;
    u8 *str = PushArray(arena, u8, cap + 1);
    u32 *ptr = in.str;
    u32 *opl = ptr + in.size;
    u64 size = 0;
    for (;ptr < opl; ptr += 1)
    {
        size += Utf8FromCodepoint(str + size, *ptr);
    }
    str[size] = 0;
    M_ArenaPop(arena, cap - size); // := ((cap + 1) - (size + 1))
    return Str8(str, size);
}

internal string32        
Str32From8(m_arena *arena, string8 in)
{
    u64 cap = in.size;
    u32 *str = PushArray(arena, u32, cap + 1);
    u8 *ptr = in.str;
    u8 *opl = ptr + in.size;
    u64 size = 0;
    decoded_codepoint consume;
    for (;ptr < opl;)
    {
        consume = DecodeCodepointFromUtf8(ptr, opl - ptr);
        ptr += consume.Advance;
        str[size] = consume.Codepoint;
        size += 1;
    }
    str[size] = 0;
    M_ArenaPop(arena, 4*(cap - size)); // := 4*((cap + 1) - (size + 1))
    string32 result = {str, size};
    return result;
}

////////////////////////////////
//~ NOTE(fakhri): Skip/Chop Helpers

internal string8
Str8ChopLastPeriod(string8 string)
{
    u64 period_pos = FindSubstr8(string, Str8Lit("."), 0, MatchFlag_FindLast);
    if(period_pos < string.size)
    {
        string.size = period_pos;
    }
    return string;
}

internal string8
Str8SkipLastSlash(string8 string)
{
    u64 slash_pos = FindSubstr8(string, Str8Lit("/"), 0,
                                MatchFlag_SlashInsensitive|
                                MatchFlag_FindLast);
    if(slash_pos < string.size)
    {
        string.str += slash_pos+1;
        string.size -= slash_pos+1;
    }
    return string;
}

internal string8
Str8SkipLastPeriod(string8 string)
{
    u64 period_pos = FindSubstr8(string, Str8Lit("."), 0, MatchFlag_FindLast);
    if(period_pos < string.size)
    {
        string.str += period_pos+1;
        string.size -= period_pos+1;
    }
    return string;
}

internal string8
Str8ChopLastSlash(string8 string)
{
    u64 slash_pos = FindSubstr8(string, Str8Lit("/"), 0,
                                MatchFlag_SlashInsensitive|
                                MatchFlag_FindLast);
    if(slash_pos < string.size)
    {
        string.size = slash_pos;
    }
    return string;
}

internal string8
Str8ChopFirstSlash(string8 string)
{
    u64 slash_pos = FindSubstr8(string, Str8Lit("/"), 0,
                                MatchFlag_SlashInsensitive);
    if(slash_pos < string.size)
    {
        string.size = slash_pos;
    }
    return string;
}

internal string8
Str8ChopFirstQuestionMark(string8 String)
{
    u64 QuestionMarkPos = FindSubstr8(String, Str8Lit("?"), 0, 0);
    if(QuestionMarkPos < String.size)
    {
        String.size = QuestionMarkPos;
    }
    return String;
}

internal string8
Str8ChopFirstOccurence(string8 String, string8 Needle)
{
    u64 NeedlePosition = FindSubstr8(String, Needle, 0, 0);
    if(NeedlePosition < String.size)
    {
        String.size = NeedlePosition;
    }
    return String;
}

internal string8
Str8ChopLeadingSpaces(string8 String)
{
    u32 Index = 0;
    while(Index < String.size && CharIsSpace(String.str[Index])) ++Index;
    String.str += Index;
    String.size -= Index;
    return String;
}

internal string8
Str8SkipToNextLine(string8 String)
{
    u64 NewLinePos = FindSubstr8(String, Str8Lit("\n"), 0, 0);
    if(NewLinePos < String.size)
    {
        String.str += NewLinePos+1;
        String.size -= NewLinePos+1;
    }
    return String;
}

////////////////////////////////
//~ NOTE(fakhri): Numeric Conversions

internal u64
u64FromStr8(string8 string, u32 radix)
{
    Assert(2 <= radix && radix <= 16);
    local_persist u8 char_to_value[] =
    {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    };
    u64 value = 0;
    for (u64 i = 0; i < string.size; i += 1){
        value *= radix;
        u8 c = string.str[i];
        value += char_to_value[(c - 0x30)&0x1F];
    }
    return value;
}

internal i64
CStyleIntFromStr8(string8 string)
{
    u64 p = 0;
    
    // consume sign
    i64 sign = +1;
    if (p < string.size){
        u8 c = string.str[p];
        if (c == '-'){
            sign = -1;
            p += 1;
        }
        else if (c == '+'){
            p += 1;
        }
    }
    
    // radix from prefix
    u64 radix = 10;
    if (p < string.size){
        u8 c0 = string.str[p];
        if (c0 == '0'){
            p += 1;
            radix = 8;
            if (p < string.size){
                u8 c1 = string.str[p];
                if (c1 == 'x'){
                    p += 1;
                    radix = 16;
                }
                else if (c1 == 'b'){
                    p += 1;
                    radix = 2;
                }
            }
        }
    }
    
    // consume integer "digits"
    string8 digits_substr = Str8SkipFirst(string, p);
    u64 n = u64FromStr8(digits_substr, (u32)radix);
    
    // combine result
    i64 result = sign*n;
    return(result);
}

internal r64
r64FromStr8(string8 string)
{
    char str[64];
    u64 str_size = string.size;
    if(str_size > sizeof(str) - 1)
    {
        str_size = sizeof(str) - 1;
    }
    MemoryCopy(str, string.str, str_size);
    str[str_size] = 0;
    return(atof(str));
}

internal string8
CStyleHexStringFromu64(m_arena *arena, u64 x, b32 caps)
{
    local_persist char int_value_to_char[] = "0123456789abcdef";
    u8 Buffer[10];
    u8 *opl = Buffer + 10;
    u8 *ptr = opl;
    if (x == 0){
        ptr -= 1;
        *ptr = '0';
    }
    else
    {
        for(;;)
        {
            u32 val = x%16;
            x /= 16;
            u8 c = (u8)int_value_to_char[val];
            if(caps)
            {
                c = CharToUpper(c);
            }
            ptr -= 1;
            *ptr = c;
            if (x == 0)
            {
                break;
            }
        }
    }
    ptr -= 1;
    *ptr = 'x';
    ptr -= 1;
    *ptr = '0';
    
    string8 result = {0};
    result.size = (u64)(ptr - Buffer);
    result.str = PushArray(arena, u8, result.size);
    MemoryCopy(result.str, Buffer, result.size);
    
    return result;
}

internal string8
Str8FromU64(m_arena *Arena, u64 X)
{
    u8 Buffer[20];
    u32 Length = 0;
    do
    {
        Length += 1;
        u8 d = X % 10;
        Buffer[20 - Length] = d + '0';
        X /= 10;
    }
    while(X);
    string8 Result = PushStr8Copy(Arena, Str8(Buffer + 20 - Length, Length));
    return Result;
}


