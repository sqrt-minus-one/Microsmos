

#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

struct font_info
{
    string8 Path;
    r32 FontScale;
};

internal void
LoadFont(game_state *GameState, m_arena *Arena, font_kind FontKind)
{
    if (FontKind == FontKind_None) return;
    Assert(FontKind < FontKind_Count);
    font *Font = GameState->Fonts + FontKind;
    MemoryZeroStruct(Font);
    struct font_info FontInfo = ZERO_STRUCT;
    switch(FontKind)
    {
        case FontKind_Arial:
        {
            FontInfo.Path = Str8Lit("data/fonts/arial.ttf");
            FontInfo.FontScale = 50.f;
        } break;
        case FontKind_GameTitle:
        {
            FontInfo.Path = Str8Lit("data/fonts/PilotCommand.otf");
            FontInfo.FontScale = 100.f;
        } break;
        case FontKind_MenuTitle:
        {
            FontInfo.Path = Str8Lit("data/fonts/PilotCommand.otf");
            FontInfo.FontScale = 60.f;
        } break;
        case FontKind_MenuItem:
        {
            FontInfo.Path = Str8Lit("data/fonts/BubbleboddyNeue.ttf");
            FontInfo.FontScale = 50.f;
        } break;
        case FontKind_InputField:
        {
            FontInfo.Path = Str8Lit("data/fonts/ariblk.ttf");
            FontInfo.FontScale = 50.f;
        } break;
        case FontKind_GameButton:
        {
            FontInfo.Path = Str8Lit("data/fonts/BubbleboddyNeue.ttf");
            FontInfo.FontScale = 30.f;
        } break;
        default: NotImplemented; break;
    }
    
    m_temp Scratch = GetScratch(&Arena, 1);
    
    // NOTE(fakhri): constants
    u32 DirectMapFirst = ' ';
    u32 DirectMapOpl = 128;
    v2r32 Oversample = { 1.f, 1.f };
    v2i32 AtlasSize = (v2i32){.X = 1024, .Y = 1024};
    
    string8 FontData = OS_LoadEntireFile(Scratch.Arena, FontInfo.Path);
    
    u8 *Pixels = PushArrayZero(Arena, u8, AtlasSize.X * AtlasSize.Y);
    Assert(Pixels && FontData.str);
    
    // NOTE(fakhri): calculate basic metrics
    r32 ascent = 0;
    r32 descent = 0;
    r32 line_gap = 0;
    stbtt_GetScaledFontVMetrics(FontData.str, 0, FontInfo.FontScale, &ascent, &descent, &line_gap);
    r32 line_advance = ascent - descent + line_gap;
    
    stbtt_pack_context ctx = {0};
    stbtt_PackBegin(&ctx, Pixels, AtlasSize.X, AtlasSize.Y, 0, 1, 0);
    stbtt_PackSetOversampling(&ctx, (u32)Oversample.X, (u32)Oversample.Y);
    stbtt_packedchar *chardata_for_range = PushArrayZero(Scratch.Arena, stbtt_packedchar, DirectMapOpl-DirectMapFirst);
    Assert(chardata_for_range);
    stbtt_pack_range rng =
    {
        FontInfo.FontScale,
        (int)DirectMapFirst,
        0,
        (int)(DirectMapFirst - DirectMapOpl),
        chardata_for_range,
    };
    stbtt_PackFontRanges(&ctx, FontData.str, 0, &rng, 1);
    stbtt_PackEnd(&ctx);
    
    // NOTE(fakhri): build direct map
    v2r32 atlas_f32 = Vec2FromVec2i32(AtlasSize);
    glyph *direct_map = PushArrayZero(Arena, glyph, DirectMapOpl-DirectMapFirst);
    Assert(direct_map);
    for(u32 codepoint = DirectMapFirst; codepoint < DirectMapOpl; codepoint += 1)
    {
        u32 index = codepoint - DirectMapFirst;
        r32 x_offset = 0;
        r32 y_offset = 0;
        stbtt_aligned_rectangle rectangle = {0};
        stbtt_GetPackedRectangle(rng.chardata_for_range, AtlasSize.Width, AtlasSize.Height, index, &x_offset, &y_offset, &rectangle, 1);
        glyph *Glyph = direct_map + index;
        Glyph->Source = RectMinMax(Vec2(rectangle.s0, rectangle.t0),
                                   Vec2(rectangle.s1, rectangle.t1));
        
        Glyph->Offset = Vec2(rectangle.x0, rectangle.y0);
        
        rectangle_2d src_px = RectMinMax(HadamardMultiplyVec2(Vec2(rectangle.s0, rectangle.t0), atlas_f32),
                                         HadamardMultiplyVec2(Vec2(rectangle.s1, rectangle.t1), atlas_f32));
        
        Glyph->Size = HadamardMultiplyVec2(RectDim(src_px),
                                           Vec2(1.f / Oversample.X, 1.f / Oversample.Y));
        Glyph->Advance = x_offset;
        if (Font->MaxAdvance < Glyph->Advance)
        {
            Font->MaxAdvance = Glyph->Advance;
        }
    }
    
    
    Font->MapFirst = DirectMapFirst;
    Font->MapFirst  = DirectMapOpl;
    Font->Map = direct_map;
    Font->Texture = ReserveTexture2D(AtlasSize, TextureFormat2D_R8);
    Font->LineAdvance = line_advance;
    Font->Ascent = ascent;
    Font->Descent = descent;
    FillTexture2D(Font->Texture, Vec2I32(0, 0), AtlasSize, Str8(Pixels, AtlasSize.X*AtlasSize.Y));
    ReleaseScratch(Scratch);
}

internal r32
GetFontMaxWidth(game_state *GameState, font_kind FontKind, u32 CharactersCount)
{
    r32 text_width = 0;
    
    font *Font = GameState->Fonts + FontKind;
    text_width = Font->MaxAdvance * CharactersCount;
    return text_width;
}

internal r32
GetFontWidth(game_state *GameState, font_kind FontKind, string8 text)
{
    r32 text_width = 0;
    font *Font = GameState->Fonts + FontKind;
    
    for (u32 ch_index = 0;
         ch_index < text.size;
         ++ch_index)
    {
        u8 ch = text.str[ch_index];
        Assert(Font->MapFirst <= ch && ch < Font->MapOPL);
        text_width += Font->Map[ch - Font->MapFirst].Advance;
    }
    
    return text_width;
}

internal inline r32
GetFontHeight(game_state *GameState, font_kind FontKind)
{
    font *Font = GameState->Fonts + FontKind;
    r32 result = Font->Ascent - Font->Descent;
    return result;
}
