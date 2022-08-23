/* date = August 19th 2022 3:40 pm */

#ifndef GAME_TEXTURE_H
#define GAME_TEXTURE_H

typedef enum
{
    TextureFormat2D_Null,
    TextureFormat2D_R8,
    TextureFormat2D_RGB8,
    TextureFormat2D_RGBA8,
    TextureFormat2D_COUNT
} texture_format_2d;

typedef struct texture_2d
{
    u32 ID;
    texture_format_2d Format;
    v2i32 Size;
} texture_2d;


typedef u32 texture_kind;
enum
{
    TextureKind_None,
    TextureKind_Count,
};

#endif //GAME_TEXTURE_H
