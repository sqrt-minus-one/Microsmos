/* date = June 20th 2022 0:42 pm */

#ifndef BASE_STRING_H
#define BASE_STRING_H

typedef struct string8 string8;
struct string8
{
    union
    {
        u8   *str;
        char *cstr;
        void *data;
    };
    u64 size;
};

typedef struct string16 string16;
struct string16
{
    u16 *str;
    u64 size;
};

typedef struct string32 string32;
struct string32
{
    u32 *str;
    u64 size;
};

typedef struct string8_node string8_node;
struct string8_node
{
    string8_node *Next;
    string8 String;
};

typedef struct string8_list string8_list;
struct string8_list
{
    string8_node *First;
    string8_node *Last;
    u64 NodeCount;
    u64 TotalSize;
};

typedef struct string_join string_join;
struct string_join
{
    string8 Pre;
    string8 Sep;
    string8 Post;
};

typedef u32 match_flags;
enum
{
    MatchFlag_CaseInsensitive  = (1<<0),
    MatchFlag_RightSideSloppy  = (1<<1),
    MatchFlag_SlashInsensitive = (1<<2),
    MatchFlag_FindLast         = (1<<3),
    MatchFlag_SkipFirst        = (1<<4),
};

typedef struct decoded_codepoint decoded_codepoint;
struct decoded_codepoint
{
    u32 Codepoint;
    u32 Advance;
};

typedef enum identifier_style
{
    identifier_style_UpperCamelCase,
    identifier_style_LowerCamelCase,
    identifier_style_UpperCase,
    identifier_style_LowerCase,
}
identifier_style;

////////////////////////////////
//~ NOTE(fakhri): Char Functions

internal b32 CharIsAlpha(u8 C);
internal b32 CharIsAlphaUpper(u8 C);
internal b32 CharIsAlphaLower(u8 C);
internal b32 CharIsDigit(u8 C);
internal b32 CharIsSymbol(u8 C);
internal b32 CharIsSpace(u8 C);
internal u8  CharToUpper(u8 C);
internal u8  CharToLower(u8 C);
internal u8  CharToForwardSlash(u8 C);

////////////////////////////////
//~ NOTE(fakhri): String Functions

//- NOTE(fakhri): Helpers
internal u64 CalculateCStringLength(char *cstr);

//- NOTE(fakhri): Basic Constructors
internal string8 Str8(u8 *str, u64 size);
#define Str8C(cstring) Str8((u8 *)(cstring), CalculateCStringLength(cstring))
#if LANG_CPP
#define Str8Lit(s) Str8((u8 *)(s), sizeof(s)-1)
#elif LANG_C
#define Str8Lit(s) ((string8){(u8 *)(s), sizeof(s)-1})
#endif
#define Str8LitComp(s) {(u8 *)(s), sizeof(s)-1}
internal string8 Str8Range(u8 *First, u8 *one_past_Last);
internal string16 Str16(u16 *str, u64 size);
internal string16 Str16C(u16 *ptr);
#define Str8Struct(ptr) Str8((u8 *)(ptr), sizeof(*(ptr)))

internal char* GetNullTerminatedCString(m_arena *Arena, string8 String);

//~ NOTE(fakhri): for format string
#define Str8Expand(s) (int)(s).size, (s).str

//- NOTE(fakhri): Substrings
internal string8 Substr8(string8 str, u64 min, u64 max);
internal string8 Str8SkipFirst(string8 str, u64 min);
internal string8 Str8ChopLast(string8 str, u64 nmax);
internal string8 Prefix8(string8 str, u64 size);
internal string8 Suffix8(string8 str, u64 size);

//- NOTE(fakhri): Matching
internal b32 Str8Match(string8 a, string8 b, match_flags flags);
internal u64 FindSubstr8(string8 haystack, string8 needle, u64 start_pos, match_flags flags);
internal b32 Str8EndsWith(string8 a, string8 b, match_flags Flags);

//- NOTE(fakhri): Allocation
internal string8 PushStr8Copy(m_arena *arena, string8 string);
internal string8 PushStr8FV(m_arena *arena, char *fmt, va_list args);
internal string8 PushStr8F(m_arena *arena, char *fmt, ...);

//- NOTE(fakhri): String Lists
internal void Str8ListPushNode(string8_list *list, string8_node *n);
internal string8_node *Str8ListPopNode(string8_list *list);
internal void Str8ListPush(m_arena *arena, string8_list *list, string8 str);
internal void Str8ListPushFV(m_arena *arena, string8_list *list, char *fmt, va_list args);
internal void Str8ListPushF(m_arena *arena, string8_list *list, char *fmt, ...);
internal void Str8ListConcat(string8_list *list, string8_list *to_push);
internal string8_list StrSplit8(m_arena *arena, string8 string, int split_count, string8 *splits);
internal string8 Str8ListJoin(m_arena *arena, string8_list list, string_join *optional_params);

//- NOTE(fakhri): String Re-Styling
internal string8 Str8Stylize(m_arena *arena, string8 string, identifier_style style, string8 separator);

////////////////////////////////
//~ NOTE(fakhri): Unicode Conversions

internal decoded_codepoint DecodeCodepointFromUtf8(u8 *str, u64 max);
internal decoded_codepoint DecodeCodepointFromUtf16(u16 *str, u64 max);
internal u32              Utf8FromCodepoint(u8 *out, u32 codepoint);
internal u32              Utf16FromCodepoint(u16 *out, u32 codepoint);
internal string8          Str8From16(m_arena *arena, string16 str);
internal string16         Str16From8(m_arena *arena, string8 str);
internal string8          Str8From32(m_arena *arena, string32 str);
internal string32         Str32From8(m_arena *arena, string8 str);

////////////////////////////////
//~ NOTE(fakhri): Skip/Chop Helpers

internal string8 Str8ChopLastPeriod(string8 str);
internal string8 Str8SkipLastSlash(string8 str);
internal string8 Str8SkipLastPeriod(string8 str);
internal string8 Str8ChopLastSlash(string8 str);
internal string8 Str8ChopFirstQuestionMark(string8 str);
internal string8 Str8ChopFirstOccurence(string8 String, string8 Needle);
internal string8 Str8ChopLeadingSpaces(string8 String);
internal string8 Str8SkipToNextLine(string8 String);

////////////////////////////////
//~ NOTE(fakhri): Numeric Conversions

internal u64 u64FromStr8(string8 str, u32 radix);
internal i64 CStyleIntFromStr8(string8 str);
internal r64 r64FromStr8(string8 str);
internal string8 CStyleHexStringFromu64(m_arena *arena, u64 x, b32 caps);

internal string8 Str8FromU64(m_arena *Arena, u64 X);

#define STB_SPRINTF_IMPLEMENTATION
#include "third_party/stb_sprintf.h"

#endif //BASE_STRING_H
