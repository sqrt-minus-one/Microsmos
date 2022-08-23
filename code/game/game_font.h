/* date = August 19th 2022 3:40 pm */

#ifndef GAME_FONT_H
#define GAME_FONT_H

typedef u32 font_kind;
enum
{
    FontKind_None,
    
    FontKind_Arial,
    FontKind_GameTitle,
    FontKind_MenuTitle,
    FontKind_MenuItem,
    FontKind_InputField,
    FontKind_GameButton,
    
    FontKind_Count,
};


typedef struct glyph
{
    rectangle_2d Source;
    v2r32 Offset;
    v2r32 Size;
    r32 Advance;
} glyph;


typedef struct font
{
    u32 MapFirst;
    u32 MapOPL;
    glyph *Map;
    
    r32 LineAdvance;
    r32 Ascent;
    r32 Descent;
    texture_2d Texture;
    r32 MaxAdvance;
} font;

#endif //GAME_FONT_H
